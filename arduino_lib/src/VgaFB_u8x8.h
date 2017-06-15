/*

VgaFB_8x8.h

U8x8 and U8g2 library integration for VgaFB (Minimalistic VGA framebuffer for microcontrollers)
https://github.com/tlaasik/vgafb/

---------------------------------------------------------------------------------------

Copyright (c) 2017, toomas.laasik@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or other
materials provided with the distribution.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------------------
*/
#ifndef _VGAFB_U8X8_H
#define _VGAFB_U8X8_H

#include <U8x8lib.h>
#include <U8g2lib.h>
#include <VgaFB_core.h>

// hide vgafb 16bit pointer into i2c_address and i2c_started.
// u8x8_t should really have void* that can point to additional state object like vgafb_t
#define VGAFB_HIDE_POINTER(u8x8, vgafb) *((uint16_t *)&(u8x8)->i2c_address) = (uint16_t)&(vgafb)
#define VGAFB_UNHIDE_POINTER(u8x8) (vgafb_t *)(*((uint16_t *)&(u8x8)->i2c_address))

class U8X8_VGAFB : public U8X8 {
protected:
	vgafb_t vgafb;
public:
	vgafb_t* getVgaFB() { return &vgafb; };
	void clearLine(uint8_t line);
	void clearDisplay(void);
	// needed to override this too or otherwise clear() would call base class clearDisplay() instead
	void clear(void);
	void scroll(int8_t tileDelta);
};

class U8X8_VGAFB_640X480_75Hz_32MHz_GENERIC_VGAFBBUS : public U8X8_VGAFB {
public: U8X8_VGAFB_640X480_75Hz_32MHz_GENERIC_VGAFBBUS(uint8_t mul, uint8_t div, uint8_t cs, uint8_t b);
};
class U8X8_VGAFB_400X300_60Hz_20MHz_GENERIC_VGAFBBUS : public U8X8_VGAFB {
public: U8X8_VGAFB_400X300_60Hz_20MHz_GENERIC_VGAFBBUS(uint8_t mul, uint8_t div, uint8_t cs, uint8_t a);
};
class U8X8_VGAFB_256X256_60Hz_20MHz_GENERIC_VGAFBBUS : public U8X8_VGAFB {
public: U8X8_VGAFB_256X256_60Hz_20MHz_GENERIC_VGAFBBUS(uint8_t mul, uint8_t div, uint8_t cs, uint8_t a);
};

class U8G2_VGAFB : public U8G2 {
protected:
	vgafb_t vgafb;
public:
	vgafb_t* getVgaFB() { return &vgafb; };
	void begin(void);
	void clearDisplay(void);
	// needed to override this too or otherwise clear() would call base class clearDisplay() instead
	void clear(void);
};
class U8G2_VGAFB_640X480_75Hz_32MHz_GENERIC_1_VGAFBBUS : public U8G2_VGAFB {
public: U8G2_VGAFB_640X480_75Hz_32MHz_GENERIC_1_VGAFBBUS(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t b);
};
class U8G2_VGAFB_400X300_60Hz_20MHz_GENERIC_1_VGAFBBUS : public U8G2_VGAFB {
public: U8G2_VGAFB_400X300_60Hz_20MHz_GENERIC_1_VGAFBBUS(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t a);
};
class U8G2_VGAFB_256X256_60Hz_20MHz_GENERIC_1_VGAFBBUS : public U8G2_VGAFB {
public: U8G2_VGAFB_256X256_60Hz_20MHz_GENERIC_1_VGAFBBUS(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t a);
};

#endif
