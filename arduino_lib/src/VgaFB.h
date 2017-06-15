/*

VgaFB.h

Minimalistic VGA framebuffer for microcontrollers
https://github.com/tlaasik/vgafb/

-------------------------------------------------------------------------------

Copyright (c) 2017, toomas.laasik@gmail.com
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
#ifndef _VGAFB_H
#define _VGAFB_H

#include <VgaFB_core.h>
#include <Arduino.h>
#include <SPI.h>

// first 4 don't need reading back
#define BLIT_CLEAR	0
#define BLIT_FILL	1
#define BLIT_SET	2
#define BLIT_INVSET	3
#define BLIT_INV	4
#define BLIT_XOR	5
#define BLIT_AND	6
#define BLIT_OR		7

#define BLIT_MAX_BYTES				9 /* allows 64px wide unaligned sprite blit */
#define BLIT_MAX_BITS				((BLIT_MAX_BYTES - 1) * 8)

class VgaFB
{
protected:
	vgafb_t vgafb;
public:
	uint16_t x, y;

	VgaFB(uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin);

	void Begin(vgamode_t mode);
	void End();
	void Enable();
	void Disable();
	void Clear();
	void Scroll(int16_t delta);

	uint16_t Width();
	uint16_t Height();
	
	void ClearLine(uint16_t line);

	void SetPixel(uint16_t x, uint16_t y, uint8_t pixel);
	uint8_t GetPixel(uint16_t x, uint16_t y);

	void BlitAlignedBytes(uint_vgafb_t offset, uint8_t* bytes, uint8_t byteCount, uint8_t startSkipBits, uint8_t endSkipBits, byte blit);
	void BlitBits(uint_vgafb_t screenOffset, uint8_t screenBitOffset, uint8_t* line, uint8_t lineBitOffset, uint8_t bitCount, byte blit);
	void Blit(uint8_t* bitmap, int16_t sx, int16_t sy, int16_t w, int16_t h, uint8_t blit);

	//void setFont(const uint8_t *font_8x8)
	//void drawGlyph(uint8_t x, uint8_t y, uint8_t encoding)
	//void drawString(uint8_t x, uint8_t y, const char *s)
};

#endif


/*DFBResult Blit (
  	IDirectFBSurface 	 	* 	thiz,
  	IDirectFBSurface 	 	* 	source,
  	const DFBRectangle 	 	* 	source_rect,
  	int 	 		x,
  	int 	 		y
);
Pass a NULL rectangle to use the whole source surface. Source may be the same surface.

DFBResult Write (
IDirectFBSurface 	 	* 	thiz,
const DFBRectangle 	 	* 	rect,
const void 	 	* 	ptr,
int 	 		pitch
);
rect defines the area inside the surface.
ptr and pitch specify the source.
The format of the surface and the source data must be the same.

DFBResult Read (
IDirectFBSurface 	 	* 	thiz,
const DFBRectangle 	 	* 	rect,
void 	 	* 	ptr,
int 	 		pitch
);
rect defines the area inside the surface to be read.
ptr and pitch specify the destination.
The destination data will have the same format as the surface.
*/