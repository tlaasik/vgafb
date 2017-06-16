/*

VgaFB_gfx.h

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
#ifndef _VGAFB_GFX_H
#define _VGAFB_GFX_H

#include <VgaFB_core.h>
#include <Adafruit_GFX.h>

class VgaFB_GFX : public Adafruit_GFX {
private:
	vgafb_t* vgafb;
	bool ownsVgafb;
public:
	VgaFB_GFX(uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin);
	VgaFB_GFX(vgafb_t* vgafb);
	~VgaFB_GFX();
	vgafb_t* getVgaFB();

	void begin(vgamode_t mode);
	void end();
  
	void display();
	void clearDisplay();

	void drawPixel(int16_t x, int16_t y, uint16_t color);

	void setPixel(int16_t x, int16_t y, uint8_t pixel);
	uint8_t getPixel(int16_t x, int16_t y);
	void scroll(int16_t lines);

	// TODO implement these
	//void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	//void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
};

#endif
