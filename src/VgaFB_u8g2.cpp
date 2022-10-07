/*

VgaFB_u8g2.cpp

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
#include <stdint.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include "VgaFB_core.h"
#include "VgaFB_u8g2.h"


// hide vgafb 16bit pointer into i2c_address and i2c_started.
// u8x8_t should really have void* that can point to additional state object like vgafb_t
#define VGAFB_HIDE_POINTER(u8x8, vgafb) *((uint16_t *)&(u8x8)->i2c_address) = (uint16_t)(vgafb)
#define VGAFB_UNHIDE_POINTER(u8x8) (vgafb_t *)(*((uint16_t *)&(u8x8)->i2c_address))


// -------------------------------- C part --------------------------------

static uint8_t u8x8_d_vgafb_generic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	uint8_t *tilePtr;
	uint16_t basePixelOffs, pixelOffs, c3;
	uint8_t repeat, r, c, j, x, y, xx, d, t, s;

	vgafb_t* vgafb = VGAFB_UNHIDE_POINTER(u8x8);

	switch (msg)
	{
		/* copied text. delete once taken into account!
		"tile_ptr" is the address of a memory area, which contains
		one or more tiles. "cnt" will contain the exact number of
		tiles in the memory areay. The size of the memory area is 8*cnt;
		Multiple tiles in the memory area form a horizontal sequence, this
		means the first tile is drawn at x_pos/y_pos, the second tile is drawn
		at x_pos+1/y_pos, third at x_pos+2/y_pos.
		"arg_int" tells how often the tile sequence should be repeated:
		For example if "cnt" is two and tile_ptr points to tiles A and B,
		then for arg_int = 3, the following tile sequence will be drawn:
		ABABAB. Totally, cnt*arg_int tiles will be drawn. */
	case U8X8_MSG_DISPLAY_DRAW_TILE:

		x = ((u8x8_tile_t *)arg_ptr)->x_pos;
		y = ((u8x8_tile_t *)arg_ptr)->y_pos;
		if (y >= u8x8->display_info->tile_height)
			break;

		basePixelOffs = vgafb->vmemFirstPixelOffset + (((uint16_t)y) << 3) * vgafb->vmemScaledStride + u8x8->x_offset;

		for (repeat = 0; repeat < arg_int; ++repeat) // repeat as many times requested
		{
			tilePtr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;
			for (r = 0; r < 8; ++r) // iterate over tile rows. we know its size is constant 8
			{
				t = ((u8x8_tile_t *)arg_ptr)->cnt;
				for (c = 0; c < t; ++c) // iterate over tile columns
				{
					xx = x + c + repeat * t;
					if (xx >= u8x8->display_info->tile_width)
						break;

					pixelOffs = basePixelOffs + (uint16_t)r * vgafb->vmemScaledStride + xx;

					// http://stackoverflow.com/questions/35544172/rotate-a-bitmap-represented-by-an-array-of-bytes
					// get first row pixels (i is row number) for anti-clockwise rotated tile
					d = 0;
					c3 = c << 3;
					for (j = 0; j < 8; ++j) {
						d <<= 1;
						d |= (tilePtr[j + c3] >> r) & 1;
					}

					s = vgafb->mode.scanlineHeight;
					while (s--) {
						VgaFB_Write(vgafb, pixelOffs, &d, 1);
						pixelOffs += vgafb->vmemStride;
					}
				}
			}
		}
		break;
	case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
		VgaFB_DisplayEnabled(vgafb, arg_int == 0);
		break;
	default:
		return 0;
	}
	return 1;
}

/* timing from VGAFB */
static const u8x8_display_info_t u8x8_vgafb_640x480_75Hz_32MHz_display_info =
{
	/* chip_enable_level = */ 0,
	/* chip_disable_level = */ 1,

	/* post_chip_enable_wait_ns = */ 25,/* ok */
	/* pre_chip_disable_wait_ns = */ 50,/* didn't quite understand from 23LCV512 datasheet if it should be 25 or 50 */
	/* reset_pulse_width_ms = */ 1, 	/* don't care, not used */
	/* post_reset_wait_ms = */ 1,       /* don't care, not used */
	/* sda_setup_time_ns = */ 25,		/* half of 20MHz clock cycle (50ns/2) */
	/* sck_pulse_width_ns = */ 25,	    /* half of 20MHz clock cycle (50ns/2) */
	/* sck_clock_hz = */ 20000000UL,	/* ignored */
	/* spi_mode = */ 0,		            /* active high, rising edge */
	/* i2c_bus_clock_100kHz = */ 1,     /* don't care. not used */
	/* data_setup_time_ns = */ 10,      /* ok */
	/* write_pulse_width_ns = */ 50,	/* didn't quite understand from 23LCV512 datasheet, but it's 50 or less */
	/* tile_width = */ 80, // 40
	/* tile_hight = */ 60, // 25
	/* default_x_offset = */ 0,			/* there's no reason to change it */
	/* flipmode_x_offset = */ 0,		/* no flipping, do don't care */
	/* pixel_width = */ 640,
	/* pixel_height = */ 480
};
static const u8x8_display_info_t u8x8_vgafb_400x300_60Hz_20MHz_display_info =
{
	/* chip_enable_level = */ 0,
	/* chip_disable_level = */ 1,

	/* post_chip_enable_wait_ns = */ 25,/* ok */
	/* pre_chip_disable_wait_ns = */ 50,/* didn't quite understand from 23LCV512 datasheet if it should be 25 or 50 */
	/* reset_pulse_width_ms = */ 1, 	/* don't care, not used */
	/* post_reset_wait_ms = */ 1,       /* don't care, not used */
	/* sda_setup_time_ns = */ 25,		/* half of 20MHz clock cycle (50ns/2) */
	/* sck_pulse_width_ns = */ 25,	    /* half of 20MHz clock cycle (50ns/2) */
	/* sck_clock_hz = */ 20000000UL,	/* should use 16MHz here? */
	/* spi_mode = */ 0,		            /* active high, rising edge */
	/* i2c_bus_clock_100kHz = */ 1,     /* don't care. not used */
	/* data_setup_time_ns = */ 10,      /* ok */
	/* write_pulse_width_ns = */ 50,	/* didn't quite understand from 23LCV512 datasheet, but it's 50 or less */
	/* tile_width = */ 50,
	/* tile_hight = */ 37,
	/* default_x_offset = */ 0,			/* there's no reason to change it */
	/* flipmode_x_offset = */ 0,		/* no flipping, do don't care */
	/* pixel_width = */ 400,
	/* pixel_height = */ 296			/* display is 300, but it's not dividable by 8, therefore using a bit lower height */
};
static const u8x8_display_info_t u8x8_vgafb_240x240_60Hz_20MHz_display_info =
{
	/* chip_enable_level = */ 0,
	/* chip_disable_level = */ 1,

	/* post_chip_enable_wait_ns = */ 25,/* ok */
	/* pre_chip_disable_wait_ns = */ 50,/* didn't quite understand from 23LCV512 datasheet if it should be 25 or 50 */
	/* reset_pulse_width_ms = */ 1, 	/* don't care, not used */
	/* post_reset_wait_ms = */ 1,       /* don't care, not used */
	/* sda_setup_time_ns = */ 25,		/* half of 20MHz clock cycle (50ns/2) */
	/* sck_pulse_width_ns = */ 25,	    /* half of 20MHz clock cycle (50ns/2) */
	/* sck_clock_hz = */ 20000000UL,	/* should use 16MHz here? */
	/* spi_mode = */ 0,		            /* active high, rising edge */
	/* i2c_bus_clock_100kHz = */ 1,     /* don't care. not used */
	/* data_setup_time_ns = */ 10,      /* ok */
	/* write_pulse_width_ns = */ 50,	/* didn't quite understand from 23LCV512 datasheet, but it's 50 or less */
	/* tile_width = */ 32,
	/* tile_hight = */ 32,
	/* default_x_offset = */ 0,			/* there's no reason to change it */
	/* flipmode_x_offset = */ 0,		/* no flipping, do don't care */
	/* pixel_width = */ 240,
	/* pixel_height = */ 240
};

uint8_t u8x8_d_vgafb_640x480_75Hz_32MHz(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	vgafb_t* vgafb = VGAFB_UNHIDE_POINTER(u8x8);
	switch (msg)
	{
	case U8X8_MSG_DISPLAY_SETUP_MEMORY:
		u8x8_d_helper_display_setup_memory(u8x8, &u8x8_vgafb_640x480_75Hz_32MHz_display_info);
		break;
	case U8X8_MSG_DISPLAY_INIT:
		u8x8_d_helper_display_init(u8x8);
		VgaFB_Begin(vgafb, vgamode_640x480_75Hz_32MHz);
		break;
	default:
		return u8x8_d_vgafb_generic(u8x8, msg, arg_int, arg_ptr);
	}
	return 1;
}
uint8_t u8x8_d_vgafb_400x300_60Hz_20MHz(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	vgafb_t* vgafb = VGAFB_UNHIDE_POINTER(u8x8);
	switch (msg)
	{
	case U8X8_MSG_DISPLAY_SETUP_MEMORY:
		u8x8_d_helper_display_setup_memory(u8x8, &u8x8_vgafb_400x300_60Hz_20MHz_display_info);
		break;
	case U8X8_MSG_DISPLAY_INIT:
		u8x8_d_helper_display_init(u8x8);
		VgaFB_Begin(vgafb, vgamode_400x300_60Hz_20MHz);
		break;
	default:
		return u8x8_d_vgafb_generic(u8x8, msg, arg_int, arg_ptr);
	}
	return 1;
}
uint8_t u8x8_d_vgafb_240x240_60Hz_20MHz(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	vgafb_t* vgafb = VGAFB_UNHIDE_POINTER(u8x8);
	switch (msg)
	{
	case U8X8_MSG_DISPLAY_SETUP_MEMORY:
		u8x8_d_helper_display_setup_memory(u8x8, &u8x8_vgafb_240x240_60Hz_20MHz_display_info);
		break;
	case U8X8_MSG_DISPLAY_INIT:
		u8x8_d_helper_display_init(u8x8);
		VgaFB_Begin(vgafb, vgamode_240x240_60Hz_20MHz);
		break;
	default:
		return u8x8_d_vgafb_generic(u8x8, msg, arg_int, arg_ptr);
	}
	return 1;
}

// only works for ARDUINO >= 10600
/*extern "C"*/ uint8_t u8x8_byte_arduino_vgafbbus(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	uint8_t *data;
	//uint8_t internal_spi_mode;
	uint16_t tcntValueBeforeWait;

	vgafb_t* vgafb = VGAFB_UNHIDE_POINTER(u8x8);

	// VgaFB uses only U8X8_MSG_BYTE_INIT, all transfers use VgaFB_Read or VgaFB_Write functions
	switch (msg)
	{
	case U8X8_MSG_BYTE_INIT:
		// disable chipselect
		u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
		//SET_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);
		// no wait required here

		SPI.usingInterrupt(0xFF);
		//SPI.usingInterrupt(13); //TIMER1_OVF_vect

		// setup hardware with SPI.begin() instead of digitalWrite() and pinMode() calls
		SPI.begin();
		return 1;
	}
	return 0;
}


// -------------------------------- U8X8 part --------------------------------

U8X8_VGAFB::U8X8_VGAFB(uint8_t mul, uint8_t div, uint8_t cs, uint8_t ab, u8x8_msg_cb display_cb) {
	this->vgafb = new vgafb_t;
	this->ownsVgafb = true;
	VgaFB_ConfigBoard(vgafb, mul, div, cs, ab);
	init(display_cb);
}
U8X8_VGAFB::U8X8_VGAFB(vgafb_t* vgafb, u8x8_msg_cb display_cb) {
	this->vgafb = vgafb;
	this->ownsVgafb = false;
	init(display_cb);
}
void U8X8_VGAFB::init(u8x8_msg_cb display_cb) {
	u8x8_t* u8x8 = getU8x8();
	u8x8_Setup(u8x8, display_cb, u8x8_cad_empty, u8x8_byte_arduino_vgafbbus, u8x8_gpio_and_delay_arduino);
	VGAFB_HIDE_POINTER(u8x8, vgafb);
}
U8X8_VGAFB::~U8X8_VGAFB() {
	if (ownsVgafb)
		delete this->vgafb;
}

vgafb_t* U8X8_VGAFB::getVgaFB() {
	return vgafb;
}
void U8X8_VGAFB::clearLine(uint8_t line) {
	// can do better than u8x8_ClearLine(&u8x8, line)
	if (line >= u8x8.display_info->tile_height)
		return;

	uint_vgafb_t offset = vgafb->vmemFirstPixelOffset + (line << 3) * vgafb->vmemScaledStride;

	VgaFB_Write(vgafb, offset, 0, vgafb->vmemScaledStride << 3);
}
void U8X8_VGAFB::clearDisplay(void) {
	// can do much better than u8x8_ClearDisplay(&u8x8)
	VgaFB_Clear(vgafb);
}
// needed to override this too or otherwise clear() would call base class clearDisplay() instead
void U8X8_VGAFB::clear(void) {
	clearDisplay();
	home();
}
void U8X8_VGAFB::scroll(int8_t tileDelta) {
	VgaFB_Scroll(vgafb, tileDelta << 3);
}

// -------------------------------- U8X8 modes --------------------------------

U8X8_VGAFB_640X480_75Hz_32MHz::U8X8_VGAFB_640X480_75Hz_32MHz(uint8_t mul, uint8_t div, uint8_t cs, uint8_t b)
	: U8X8_VGAFB(mul, div, cs, b, u8x8_d_vgafb_640x480_75Hz_32MHz) {}
U8X8_VGAFB_640X480_75Hz_32MHz::U8X8_VGAFB_640X480_75Hz_32MHz(vgafb_t* vgafb)
	: U8X8_VGAFB(vgafb, u8x8_d_vgafb_640x480_75Hz_32MHz) {}

U8X8_VGAFB_400X300_60Hz_20MHz::U8X8_VGAFB_400X300_60Hz_20MHz(uint8_t mul, uint8_t div, uint8_t cs, uint8_t a)
	: U8X8_VGAFB(mul, div, cs, a, u8x8_d_vgafb_400x300_60Hz_20MHz) {}
U8X8_VGAFB_400X300_60Hz_20MHz::U8X8_VGAFB_400X300_60Hz_20MHz(vgafb_t* vgafb)
	: U8X8_VGAFB(vgafb, u8x8_d_vgafb_400x300_60Hz_20MHz) {}

U8X8_VGAFB_240X240_60Hz_20MHz::U8X8_VGAFB_240X240_60Hz_20MHz(uint8_t mul, uint8_t div, uint8_t cs, uint8_t a)
	: U8X8_VGAFB(mul, div, cs, a, u8x8_d_vgafb_240x240_60Hz_20MHz) {}
U8X8_VGAFB_240X240_60Hz_20MHz::U8X8_VGAFB_240X240_60Hz_20MHz(vgafb_t* vgafb)
	: U8X8_VGAFB(vgafb, u8x8_d_vgafb_240x240_60Hz_20MHz) {}


// -------------------------------- U8g2 part --------------------------------

U8G2_VGAFB::U8G2_VGAFB(uint8_t mul, uint8_t div, uint8_t cs, uint8_t ab, const u8g2_cb_t *rotation, u8x8_msg_cb display_cb, uint8_t* buf) {
	this->vgafb = new vgafb_t;
	this->ownsVgafb = true;
	VgaFB_ConfigBoard(vgafb, mul, div, cs, ab);
	init(rotation, display_cb, buf);
}
U8G2_VGAFB::U8G2_VGAFB(vgafb_t* vgafb, const u8g2_cb_t *rotation, u8x8_msg_cb display_cb, uint8_t* buf) {
	this->vgafb = vgafb;
	this->ownsVgafb = false;
	init(rotation, display_cb, buf);
}
void U8G2_VGAFB::init(const u8g2_cb_t *rotation, u8x8_msg_cb display_cb, uint8_t* buf) {
	u8g2_SetupDisplay(&u8g2, display_cb, u8x8_cad_empty, u8x8_byte_arduino_vgafbbus, u8x8_gpio_and_delay_arduino);
	u8g2_SetupBuffer(&u8g2, buf, 1, u8g2_ll_hvline_vertical_top_lsb, rotation);
	// ODO what does u8g2_ll_hvline_vertical_top_lsb do? there are alternatives

	u8x8_t* u8x8 = getU8x8();
	VGAFB_HIDE_POINTER(u8x8, vgafb);
}
U8G2_VGAFB::~U8G2_VGAFB() {
	if (ownsVgafb)
		delete this->vgafb;
}
void U8G2_VGAFB::clearDisplay() {
	// can do much better than u8g2_ClearDisplay(&u8g2)
	VgaFB_Clear(vgafb);
}
vgafb_t* U8G2_VGAFB::getVgaFB() {
	return vgafb;
}
// needed to override begin() and clear() or otherwise clear() would call base class clearDisplay() instead
void U8G2_VGAFB::begin() {
	initDisplay();
	clearDisplay();
	setPowerSave(0);
}
void U8G2_VGAFB::clear() {
	home();
	clearDisplay();
	clearBuffer();
}


// -------------------------------- U8g2 modes --------------------------------

U8G2_VGAFB_640X480_75Hz_32MHz_1::U8G2_VGAFB_640X480_75Hz_32MHz_1(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t b)
	: U8G2_VGAFB(mul, div, cs, b, rotation, u8x8_d_vgafb_640x480_75Hz_32MHz, buf) {}
U8G2_VGAFB_640X480_75Hz_32MHz_1::U8G2_VGAFB_640X480_75Hz_32MHz_1(const u8g2_cb_t *rotation, vgafb_t* vgafb)
	: U8G2_VGAFB(vgafb, rotation, u8x8_d_vgafb_640x480_75Hz_32MHz, buf) {}

U8G2_VGAFB_400X300_60Hz_20MHz_1::U8G2_VGAFB_400X300_60Hz_20MHz_1(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t a)
	: U8G2_VGAFB(mul, div, cs, a, rotation, u8x8_d_vgafb_400x300_60Hz_20MHz, buf) {}
U8G2_VGAFB_400X300_60Hz_20MHz_1::U8G2_VGAFB_400X300_60Hz_20MHz_1(const u8g2_cb_t *rotation, vgafb_t* vgafb)
	: U8G2_VGAFB(vgafb, rotation, u8x8_d_vgafb_400x300_60Hz_20MHz, buf) {}

U8G2_VGAFB_240X240_60Hz_20MHz_1::U8G2_VGAFB_240X240_60Hz_20MHz_1(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t a)
	: U8G2_VGAFB(mul, div, cs, a, rotation, u8x8_d_vgafb_240x240_60Hz_20MHz, buf) {}
U8G2_VGAFB_240X240_60Hz_20MHz_1::U8G2_VGAFB_240X240_60Hz_20MHz_1(const u8g2_cb_t *rotation, vgafb_t* vgafb)
	: U8G2_VGAFB(vgafb, rotation, u8x8_d_vgafb_240x240_60Hz_20MHz, buf) {}
