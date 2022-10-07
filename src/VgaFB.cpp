/*

VgaFB.cpp

Minimalistic VGA framebuffer for microcontrollers
https://github.com/tlaasik/vgafb/

-------------------------------------------------------------------------------

Copyright (c) 2022, toomas.laasik@gmail.com
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include "VgaFB_core.h"
#include "VgaFB.h"

VgaFB::VgaFB(uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin) {
	vgafb = new vgafb_t; // if it fails, then contructor is not called, but that's ok
	ownsVgafb = true;
	VgaFB_ConfigBoard(vgafb, mul, div, cs_pin, ab_pin);
}
VgaFB::VgaFB(vgafb_t* vgafb) {
	this->vgafb = vgafb;
	ownsVgafb = false;
}
VgaFB::~VgaFB() {
	if (ownsVgafb)
		delete vgafb;
}
vgafb_t* VgaFB::getVgaFB() {
	return vgafb;
}

void VgaFB::begin(vgamode_t mode) {
	VgaFB_Begin(vgafb, mode);
}
void VgaFB::end() {
	VgaFB_End(vgafb);
}

void VgaFB::enable() {
	VgaFB_DisplayEnabled(vgafb, true);
}
void VgaFB::disable() {
	VgaFB_DisplayEnabled(vgafb, false);
}

void VgaFB::clear() {
	VgaFB_Clear(vgafb);
}
void VgaFB::clearLine(uint16_t line) {
	if (line >= vgafb->vVisibleScaled)
		return;
	VgaFB_Write(vgafb, vgafb->vmemFirstPixelOffset + line * vgafb->vmemScaledStride, 0, vgafb->vmemScaledStride);
}
void VgaFB::scroll(int16_t delta) {
	VgaFB_Scroll(vgafb, delta);
}

uint16_t VgaFB::getWidth() {
	return vgafb->mode.hVisible;
}
uint16_t VgaFB::getHeight() {
	return vgafb->vVisibleScaled;
}

void VgaFB::setPixel(int16_t x, int16_t y, uint8_t pixel) {
	VgaFB_SetPixel(vgafb, x, y, pixel);
}
uint8_t VgaFB::getPixel(int16_t x, int16_t y) {
	return VgaFB_GetPixel(vgafb, x, y);
}

// arg restrictions (if not followed buffer overrun will occur):
//  startSkipBits 0..7
//  endSkipBits 0..7
//  byteCount 0..VGAFB_MAX_SPI_TRANSACTION_BYTES
void VgaFB::blitAlignedBytes(uint_vgafb_t offset, uint8_t* bytes, uint8_t byteCount, uint8_t startSkipBits, uint8_t endSkipBits, blitmode_t blit)
{
	if (byteCount <= 0) return;

	uint8_t buf[VGAFB_MAX_SPI_TRANSACTION_BYTES];
	uint8_t mask[VGAFB_MAX_SPI_TRANSACTION_BYTES]; // only used when startSkipBits or endSkipBits are set

	// build mask
	register uint8_t c = byteCount;
	while (c--) mask[c] = 0xFF;
	if (startSkipBits | endSkipBits) {
		mask[0] = 0x0FF >> startSkipBits;
		mask[byteCount - 1] &= 0x0FF << endSkipBits;
	}

	if (blit >= BLIT_INV || (startSkipBits | endSkipBits))
		VgaFB_Read(vgafb, offset, buf, byteCount);

	uint8_t i = vgafb->mode.scanlineHeight;
	while (i--)
	{
		switch (blit) {
		case BLIT_CLEAR:
			for (c = 0; c < byteCount; c++)
				buf[c] &= ~mask[c];
			break;
		case BLIT_FILL:
			for (c = 0; c < byteCount; c++)
				buf[c] |= mask[c];
			break;
		case BLIT_SET:
			for (c = 0; c < byteCount; c++)
				buf[c] = (bytes[c] & mask[c]) | (buf[c] & ~mask[c]);
			break;
		case BLIT_INVSET:
			for (c = 0; c < byteCount; c++)
				buf[c] = (~bytes[c] & mask[c]) | (buf[c] & ~mask[c]);
			break;
		case BLIT_INV:
			for (c = 0; c < byteCount; c++)
				buf[c] = buf[c] ^ mask[c];
			break;
		case BLIT_XOR:
			for (c = 0; c < byteCount; c++)
				buf[c] = ((bytes[c] ^ buf[c]) & mask[c]) | (buf[c] & ~mask[c]);
			break;
		case BLIT_AND:
			for (c = 0; c < byteCount; c++)
				buf[c] = ((bytes[c] & buf[c]) & mask[c]) | (buf[c] & ~mask[c]);
			break;
		case BLIT_OR:
			for (c = 0; c < byteCount; c++)
				buf[c] = ((bytes[c] | buf[c]) & mask[c]) | (buf[c] & ~mask[c]);
			break;
		}

		VgaFB_Write(vgafb, offset, buf, byteCount);

		offset += vgafb->vmemStride;
	}

	// approximation
	if (blit == BLIT_CLEAR) {
		offset -= vgafb->vmemStride;
	}
	else {
		offset -= vgafb->vmemStride + byteCount;
	}
	if (offset > vgafb->vmemLastPixelOffset)
		vgafb->vmemLastPixelOffset = offset;
}

// arg restrictions (if not followed buffer overrun will occur):
//  sBitOffset 0..7
//  lineBitOffset 0..7
//  bitCount 0..((VGAFB_MAX_SPI_TRANSACTION_BYTES - 1) * 8)
void VgaFB::blitBits(uint_vgafb_t screenOffset, uint8_t screenBitOffset, uint8_t* line, uint8_t lineBitOffset, uint8_t bitCount, blitmode_t blit)
{
	if (bitCount <= 0) return;

	uint8_t bitbuf[VGAFB_MAX_SPI_TRANSACTION_BYTES];
	uint8_t* bitbufPtr = bitbuf;
	uint8_t byteCount = ((screenBitOffset + bitCount - 1) >> 3) + 1;

	if (blit >= BLIT_SET && blit != BLIT_INV)
	{
		// if need to shift left
		if (screenBitOffset < lineBitOffset)
		{
			uint8_t lshift = lineBitOffset - screenBitOffset;
			for (uint8_t x = 0; x < byteCount; x++)
				bitbuf[x] = (line[x] << lshift) | (line[x + 1] >> (8 - lshift));
		}
		// if need to shift right
		else if (screenBitOffset > lineBitOffset)
		{
			uint8_t rshift = screenBitOffset - lineBitOffset;
			bitbuf[0] = line[0] >> rshift;
			for (uint8_t x = 1; x < byteCount; x++)
				bitbuf[x] = (line[x] >> rshift) | (line[x - 1] << (8 - rshift));
		}
		// no shifting needed
		else {
			bitbufPtr = line;
		}
	}

	uint8_t endSkipBits = (byteCount << 3) - (screenBitOffset + bitCount);
	blitAlignedBytes(screenOffset, bitbufPtr, byteCount, screenBitOffset, endSkipBits, blit);
}

void VgaFB::blit(uint8_t* bitmap, int16_t sx, int16_t sy, int16_t w, int16_t h, blitmode_t blit)
{
	// return if there are no pixels to blit
	if (w <= 0 || h <= 0)
		return;

	int16_t bx = 0; // bitmap x and y, >=0
	int16_t by = 0;
	int16_t bw = w; // bw and bh won't change
	int16_t bh = h;

	// left clamping
	if (sx < 0) {
		w += sx;
		if (w <= 0) return; // all bitmap pixels are left from screen edge
		bx = -sx;
		sx = 0;
	}
	else if (sx >= vgafb->mode.hVisible) return; // all bitmap pixels are right from screen edge
	// right clamping
	if (sx + w > vgafb->mode.hVisible)
		w = vgafb->mode.hVisible - sx;

	// top clamping
	if (sy < 0) {
		h += sy;
		if (h <= 0) return; // all bitmap pixels are top from screen edge
		by = -sy;
		sy = 0;
	}
	else if (sy >= vgafb->vVisibleScaled) return; // all bitmap pixels are bottom from screen edge
	// bottom clamping
	if (sy + h > vgafb->vVisibleScaled)
		h = vgafb->vVisibleScaled - sy;

	for (int16_t y = 0; y < h; y++)
	{
		int16_t csy = sy + y;
		int16_t cby = by + y;

		uint_vgafb_t sOffset = vgafb->vmemFirstPixelOffset + (uint16_t)csy * vgafb->vmemScaledStride + ((uint16_t)sx >> 3);
		uint8_t sBitOffset = sx & 0x07;

		// XXX can't use the same technique for reducing integer sizes as with screen, because
		// the bitmap size may not be 8-bit aligned as screen is. If faster code is needed
		// then we could use uint16_t here and accept than max bitmap size is about 320x200
		uint32_t bBitPtr = (uint16_t)cby * (uint16_t)bw + (uint16_t)bx;

		int16_t ww = w;
		while (ww > 0) {
			uint8_t www = ww > 64 ? 64 : ww;
			blitBits(sOffset, sBitOffset, bitmap + (bBitPtr >> 3), bBitPtr & 0x07, www, blit);
			sOffset += 8;
			bBitPtr += 64;
			ww -= 64;
		}
	}
}
