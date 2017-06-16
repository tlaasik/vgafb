/*

VgaFB_u8g2.h

U8x8 and U8g2 library integration for VgaFB (Minimalistic VGA framebuffer for microcontrollers)
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
#ifndef _VGAFB_U8G2_H
#define _VGAFB_U8G2_H

#include <U8x8lib.h>
#include <U8g2lib.h>
#include <VgaFB_core.h>

// hide vgafb 16bit pointer into i2c_address and i2c_started.
// u8x8_t should really have void* that can point to additional state object like vgafb_t
#define VGAFB_HIDE_POINTER(u8x8, vgafb) *((uint16_t *)&(u8x8)->i2c_address) = (uint16_t)(vgafb)
#define VGAFB_UNHIDE_POINTER(u8x8) (vgafb_t *)(*((uint16_t *)&(u8x8)->i2c_address))

class U8X8_VGAFB : public U8X8 {
protected:
	vgafb_t* vgafb;
public:
	vgafb_t* getVgaFB() { return vgafb; };
	void clearLine(uint8_t line);
	void clearDisplay(void);
	// needed to override this too or otherwise clear() would call base class clearDisplay() instead
	void clear(void);
	void scroll(int8_t tileDelta);
};

class U8X8_VGAFB_640X480_75Hz_32MHz : public U8X8_VGAFB {
public: U8X8_VGAFB_640X480_75Hz_32MHz(uint8_t mul, uint8_t div, uint8_t cs, uint8_t b);
};
class U8X8_VGAFB_400X300_60Hz_20MHz : public U8X8_VGAFB {
public: U8X8_VGAFB_400X300_60Hz_20MHz(uint8_t mul, uint8_t div, uint8_t cs, uint8_t a);
};
class U8X8_VGAFB_256X256_60Hz_20MHz : public U8X8_VGAFB {
public: U8X8_VGAFB_256X256_60Hz_20MHz(uint8_t mul, uint8_t div, uint8_t cs, uint8_t a);
};

class U8G2_VGAFB : public U8G2 {
protected:
	vgafb_t* vgafb;
	bool ownsVgafb;
private:
	void init(const u8g2_cb_t *rotation, u8x8_msg_cb display_cb, uint8_t* buf);
public:
	U8G2_VGAFB(uint8_t mul, uint8_t div, uint8_t cs, uint8_t ab, const u8g2_cb_t *rotation, u8x8_msg_cb display_cb, uint8_t* buf);
	U8G2_VGAFB(vgafb_t* vgafb, const u8g2_cb_t *rotation, u8x8_msg_cb display_cb, uint8_t* buf);
	~U8G2_VGAFB();

	vgafb_t* getVgaFB() { return vgafb; };
	void begin(void);
	void clearDisplay(void);
	// needed to override this too or otherwise clear() would call base class clearDisplay() instead
	void clear(void);
};

class U8G2_VGAFB_640X480_75Hz_32MHz_1 : public U8G2_VGAFB {
private:
	uint8_t buf[640];
public:
	U8G2_VGAFB_640X480_75Hz_32MHz_1(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t b);
	U8G2_VGAFB_640X480_75Hz_32MHz_1(const u8g2_cb_t *rotation, vgafb_t* vgafb);
};

class U8G2_VGAFB_400X300_60Hz_20MHz_1 : public U8G2_VGAFB {
private:
	uint8_t buf[400];
public:
	U8G2_VGAFB_400X300_60Hz_20MHz_1(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t a);
	U8G2_VGAFB_400X300_60Hz_20MHz_1(const u8g2_cb_t *rotation, vgafb_t* vgafb);
};

class U8G2_VGAFB_256X256_60Hz_20MHz_1 : public U8G2_VGAFB {
private:
	uint8_t buf[256];
public:
	U8G2_VGAFB_256X256_60Hz_20MHz_1(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t a);
	U8G2_VGAFB_256X256_60Hz_20MHz_1(const u8g2_cb_t *rotation, vgafb_t* vgafb);
};

#endif
