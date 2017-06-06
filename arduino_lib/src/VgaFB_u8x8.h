#ifndef _VGAFB_U8X8_H
#define _VGAFB_U8X8_H

#include <U8x8lib.h>
#include <U8g2lib.h>
#include <VgaFB_core.h>

// reusing pin indexes with more appropriate names
#define U8X8_PIN_VGAFB_HSYNC		U8X8_PIN_D4
#define U8X8_PIN_VGAFB_VSYNC		U8X8_PIN_D5
#define U8X8_PIN_VGAFB_AB			U8X8_PIN_D6
//#define U8X8_MSG_GPIO_VGAFB_HSYNC		U8X8_MSG_GPIO(U8X8_PIN_VGAFB_HSYNC)
//#define U8X8_MSG_GPIO_VGAFB_VSYNC		U8X8_MSG_GPIO(U8X8_PIN_VGAFB_VSYNC)
//#define U8X8_MSG_GPIO_VGAFB_AB		U8X8_MSG_GPIO(U8X8_PIN_VGAFB_AB)

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
public: U8X8_VGAFB_640X480_75Hz_32MHz_GENERIC_VGAFBBUS(uint8_t mul, uint8_t div, uint8_t cs, uint8_t b, uint8_t reset = U8X8_PIN_NONE);
};
class U8X8_VGAFB_400X300_60Hz_20MHz_GENERIC_VGAFBBUS : public U8X8_VGAFB {
public: U8X8_VGAFB_400X300_60Hz_20MHz_GENERIC_VGAFBBUS(uint8_t mul, uint8_t div, uint8_t cs, uint8_t ab, uint8_t reset = U8X8_PIN_NONE);
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
public: U8G2_VGAFB_640X480_75Hz_32MHz_GENERIC_1_VGAFBBUS(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t b, uint8_t reset = U8X8_PIN_NONE);
};
class U8G2_VGAFB_400X300_60Hz_20MHz_GENERIC_1_VGAFBBUS : public U8G2_VGAFB {
public: U8G2_VGAFB_400X300_60Hz_20MHz_GENERIC_1_VGAFBBUS(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t ab, uint8_t reset = U8X8_PIN_NONE);
};

#endif
