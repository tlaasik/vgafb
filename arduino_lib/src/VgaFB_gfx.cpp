/*

VgaFB_gfx.cpp

Adafruit GFX library integration for VgaFB (Minimalistic VGA framebuffer for microcontrollers)
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
#include "VgaFB_core.h"
#include "VgaFB_gfx.h"

VgaFB_GFX::VgaFB_GFX(uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin) : Adafruit_GFX(0, 0)
{
	vgafb = new vgafb_t; // if it fails, then contructor is not called, but that's ok
	ownsVgafb = true;
	VgaFB_ConfigBoard(vgafb, mul, div, cs_pin, ab_pin);
}
VgaFB_GFX::~VgaFB_GFX()
{
	if(ownsVgafb)
		delete vgafb;
}
VgaFB_GFX::VgaFB_GFX(vgafb_t* vgafb) : Adafruit_GFX(vgafb->mode.hTotal, vgafb->mode.vTotal)
{
	this->vgafb = vgafb;
	ownsVgafb = false;
}
vgafb_t* VgaFB_GFX::getVgaFB()
{
	return vgafb;
}

void VgaFB_GFX::begin(vgamode_t mode)
{
	_width = mode.hVisible;
	_height = mode.vVisible;
	VgaFB_Begin(vgafb, mode);
	VgaFB_DisplayEnabled(vgafb, false);
}

void VgaFB_GFX::end()
{
	VgaFB_End(vgafb);
}

void VgaFB_GFX::display()
{
	VgaFB_DisplayEnabled(vgafb, true);
}

void VgaFB_GFX::clearDisplay()
{
	VgaFB_DisplayEnabled(vgafb, false);
	VgaFB_Clear(vgafb);
}

void VgaFB_GFX::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	VgaFB_SetPixel(vgafb, x, y, color ? 1 : 0);
}
void VgaFB_GFX::setPixel(int16_t x, int16_t y, uint8_t pixel)
{
	VgaFB_SetPixel(vgafb, x, y, pixel);
}

uint8_t VgaFB_GFX::getPixel(int16_t x, int16_t y)
{
	return VgaFB_GetPixel(vgafb, x, y);
}

void VgaFB_GFX::scroll(int16_t lines)
{
	return VgaFB_Scroll(vgafb, lines);
}

// TODO it's possible to write optimized code that uses VgaFB_Write
/*
void VgaFB_GFX::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
}
void VgaFB_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
}
*/
