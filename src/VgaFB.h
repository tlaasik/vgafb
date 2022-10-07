/*

VgaFB.h

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
#ifndef _VGAFB_H
#define _VGAFB_H

#include <stdint.h>
#include "VgaFB_core.h"

/**
 * Image blit modes. first 4 don't need reading back
 */
enum blitmode_t {
	BLIT_CLEAR = 0,
	BLIT_FILL = 1,
	BLIT_SET = 2,
	BLIT_INVSET = 3,
	BLIT_INV = 4,
	BLIT_XOR = 5,
	BLIT_AND = 6,
	BLIT_OR = 7
};

class VgaFB
{
private:
	vgafb_t* vgafb;
	bool ownsVgafb;
	void blitAlignedBytes(uint_vgafb_t offset, uint8_t* bytes, uint8_t byteCount, uint8_t startSkipBits, uint8_t endSkipBits, blitmode_t blit);
	void blitBits(uint_vgafb_t screenOffset, uint8_t screenBitOffset, uint8_t* line, uint8_t lineBitOffset, uint8_t bitCount, blitmode_t blit);
public:
	uint16_t x, y;

	VgaFB(uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin);
	VgaFB(vgafb_t* vgafb);
	~VgaFB();
	vgafb_t* getVgaFB();

	void begin(vgamode_t mode);
	void end();

	void enable();
	void disable();

	void clear();
	void clearLine(uint16_t line);
	void scroll(int16_t delta);

	uint16_t getWidth();
	uint16_t getHeight();

	void setPixel(int16_t x, int16_t y, uint8_t pixel);
	uint8_t getPixel(int16_t x, int16_t y);
	
	void blit(uint8_t* bitmap, int16_t sx, int16_t sy, int16_t w, int16_t h, blitmode_t blit);
};

#endif
