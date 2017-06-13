
#include "VgaFB_core.h"
#include "VgaFB_u8x8.h"
#include <U8x8lib.h>

static uint8_t u8x8_d_vgafb_generic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	uint8_t *tilePtr;
	uint16_t basePixelOffs, pixelOffs, ramAddr, c3;
	uint8_t repeat, r, c, j, x, y, xx, yy, k, d, t;
	uint8_t buf[4];

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

					VgaFB_Write(vgafb, pixelOffs, &d, 1);
				}
			}
		}
		break;
		/* TODO disable syncs and pixel clocking?
		case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
		// displayOn = arg_int==0;
		break;
		*/
	default:
		return 0;
	}
	return 1;
}

/* timing from VGAFB */
static const u8x8_display_info_t u8x8_vgafb_640x480_75Hz_32MHz_generic_display_info =
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
static const u8x8_display_info_t u8x8_vgafb_400x300_60Hz_20MHz_generic_display_info =
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

uint8_t u8x8_d_vgafb_640x480_75Hz_32MHz_generic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	vgafb_t* vgafb = VGAFB_UNHIDE_POINTER(u8x8);
	switch (msg)
	{
	case U8X8_MSG_DISPLAY_SETUP_MEMORY:
		u8x8_d_helper_display_setup_memory(u8x8, &u8x8_vgafb_640x480_75Hz_32MHz_generic_display_info);
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
// 16MHz OK, aga sync on modifitseeritud. mult 5/4
// VESA 800x600@60Hz hsync: 37.9kHz 40.0  800 840 968 1056  600 601 605 628  +hsync +vsync
uint8_t u8x8_d_vgafb_400x300_60Hz_20MHz_generic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	vgafb_t* vgafb = VGAFB_UNHIDE_POINTER(u8x8);
	switch (msg)
	{
	case U8X8_MSG_DISPLAY_SETUP_MEMORY:
		u8x8_d_helper_display_setup_memory(u8x8, &u8x8_vgafb_400x300_60Hz_20MHz_generic_display_info);
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
		//u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
		SET_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);
		// no wait required here

		SPI.usingInterrupt(0xFF);
		//SPI.usingInterrupt(13); //TIMER1_OVF_vect

		// setup hardware with SPI.begin() instead of digitalWrite() and pinMode() calls
		SPI.begin();
		return 1;
	}
	return 0;
}

void u8x8_SetPin_VGAFBBUS(u8x8_t *u8x8, uint8_t mul, uint8_t div, uint8_t cs, uint8_t ab, uint8_t reset)
{
	u8x8_SetPin(u8x8, U8X8_PIN_CS, cs);
	u8x8_SetPin(u8x8, U8X8_PIN_VGAFB_HSYNC, hSync);
	u8x8_SetPin(u8x8, U8X8_PIN_VGAFB_VSYNC, vSync);
	u8x8_SetPin(u8x8, U8X8_PIN_VGAFB_AB, ab);
	u8x8_SetPin(u8x8, U8X8_PIN_RESET, reset);

	vgafb_t* vgafb = VGAFB_UNHIDE_POINTER(u8x8);
	VgaFB_ConfigBoard(vgafb, mul, div, cs, ab);
}

void U8X8_VGAFB::clearLine(uint8_t line) {
	// can do better than u8x8_ClearLine(&u8x8, line)
	if (line >= u8x8.display_info->tile_height)
		return;

	// can't delete in one block, because VgaFB_Write max cnt is 255. clearing 8 lines
	uint16_t base = line << 3;
	uint8_t cnt = 8; // tile height in lines
	while (cnt--)
	{
		uint_vgafb_t offset = vgafb.vmemFirstPixelOffset +
			(base * vgafb.mode.scanlineHeight + vgafb.mode.vTotal - vgafb.mode.vSyncEnd) * vgafb.vmemStride;

		VgaFB_Write(&vgafb, offset, 0, vgafb.vmemStride);
	}
}
void U8X8_VGAFB::clearDisplay(void) {
	// can do much better than u8x8_ClearDisplay(&u8x8)
	VgaFB_Clear(&vgafb);
}
// needed to override this too or otherwise clear() would call base class clearDisplay() instead
void U8X8_VGAFB::clear(void) {
	clearDisplay();
	home();
}
void U8X8_VGAFB::scroll(int8_t tileDelta) {
	VgaFB_Scroll(&vgafb, tileDelta << 3);
}

U8X8_VGAFB_640X480_75Hz_32MHz_GENERIC_VGAFBBUS::U8X8_VGAFB_640X480_75Hz_32MHz_GENERIC_VGAFBBUS(uint8_t mul, uint8_t div, uint8_t cs, uint8_t b, uint8_t reset) : U8X8_VGAFB() {
	u8x8_t* u8x8 = getU8x8();
	u8x8_Setup(u8x8, u8x8_d_vgafb_640x480_75Hz_32MHz_generic, u8x8_cad_empty, u8x8_byte_arduino_vgafbbus, u8x8_gpio_and_delay_arduino);
	VGAFB_HIDE_POINTER(u8x8, vgafb);
	u8x8_SetPin_VGAFBBUS(u8x8, mul, div, cs, b, reset);
}
U8X8_VGAFB_400X300_60Hz_20MHz_GENERIC_VGAFBBUS::U8X8_VGAFB_400X300_60Hz_20MHz_GENERIC_VGAFBBUS(uint8_t mul, uint8_t div, uint8_t cs, uint8_t a, uint8_t reset) : U8X8_VGAFB() {
	u8x8_t* u8x8 = getU8x8();
	u8x8_Setup(u8x8, u8x8_d_vgafb_400x300_60Hz_20MHz_generic, u8x8_cad_empty, u8x8_byte_arduino_vgafbbus, u8x8_gpio_and_delay_arduino);
	VGAFB_HIDE_POINTER(u8x8, vgafb);
	u8x8_SetPin_VGAFBBUS(u8x8, mul, div, cs, a, reset);
}

void U8G2_VGAFB::clearDisplay(void) {
	// can do much better than u8g2_ClearDisplay(&u8g2)
	VgaFB_Clear(&vgafb);
}
// needed to override begin() and clear() or otherwise clear() would call base class clearDisplay() instead
void U8G2_VGAFB::begin(void) {
	initDisplay(); clearDisplay(); setPowerSave(0);
}
void U8G2_VGAFB::clear(void) {
	home(); clearDisplay(); clearBuffer();
}

uint8_t *u8g2_m_vgafb_50_1(uint8_t *page_cnt)
{
	static uint8_t buf[400];
	*page_cnt = 1;
	return buf;
}
uint8_t *u8g2_m_vgafb_80_1(uint8_t *page_cnt)
{
	static uint8_t buf[640];
	*page_cnt = 1;
	return buf;
}
void u8g2_Setup_vgafb_400x300_60Hz_20MHz_generic_1(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb)
{
	uint8_t tile_buf_height;
	uint8_t *buf;
	u8g2_SetupDisplay(u8g2, u8x8_d_vgafb_400x300_60Hz_20MHz_generic, u8x8_cad_empty, byte_cb, gpio_and_delay_cb);
	buf = u8g2_m_vgafb_50_1(&tile_buf_height);
	u8g2_SetupBuffer(u8g2, buf, tile_buf_height, u8g2_ll_hvline_vertical_top_lsb, rotation);
	// ODO what does u8g2_ll_hvline_vertical_top_lsb do? tere are alternatives
}
void u8g2_Setup_vgafb_640x480_75Hz_32MHz_generic_1(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb)
{
	uint8_t tile_buf_height;
	uint8_t *buf;
	u8g2_SetupDisplay(u8g2, u8x8_d_vgafb_640x480_75Hz_32MHz_generic, u8x8_cad_empty, byte_cb, gpio_and_delay_cb);
	buf = u8g2_m_vgafb_80_1(&tile_buf_height);
	u8g2_SetupBuffer(u8g2, buf, tile_buf_height, u8g2_ll_hvline_vertical_top_lsb, rotation);
	// ODO what does u8g2_ll_hvline_vertical_top_lsb do? tere are alternatives
}

U8G2_VGAFB_640X480_75Hz_32MHz_GENERIC_1_VGAFBBUS::U8G2_VGAFB_640X480_75Hz_32MHz_GENERIC_1_VGAFBBUS(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t b, uint8_t reset) : U8G2_VGAFB() {
	u8g2_Setup_vgafb_640x480_75Hz_32MHz_generic_1(&u8g2, rotation, u8x8_byte_arduino_vgafbbus, u8x8_gpio_and_delay_arduino);
	u8x8_t* u8x8 = getU8x8();
	VGAFB_HIDE_POINTER(u8x8, vgafb);
	u8x8_SetPin_VGAFBBUS(u8x8, mul, div, cs, b, reset);
};
U8G2_VGAFB_400X300_60Hz_20MHz_GENERIC_1_VGAFBBUS::U8G2_VGAFB_400X300_60Hz_20MHz_GENERIC_1_VGAFBBUS(const u8g2_cb_t *rotation, uint8_t mul, uint8_t div, uint8_t cs, uint8_t a, uint8_t reset) : U8G2_VGAFB() {
	u8g2_Setup_vgafb_400x300_60Hz_20MHz_generic_1(&u8g2, rotation, u8x8_byte_arduino_vgafbbus, u8x8_gpio_and_delay_arduino);
	u8x8_t* u8x8 = getU8x8();
	VGAFB_HIDE_POINTER(u8x8, vgafb);
	u8x8_SetPin_VGAFBBUS(u8x8, mul, div, cs, a, reset);
}
