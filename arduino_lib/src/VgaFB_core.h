/*

VgaFB_core.h

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

This is all low level C code. For practical purposes you should normally use classes
like VgaFB, U8X8_VGAFB..., U8g2_VGAFB... or any other higher level code that does
font rendering, sprite rendering and other useful stuff.

vgafb_t v;
VgaFB_ConfigBoard(&v,  1, 1,  4, 8);
VgaFB_Begin(&v, vgamode_640x480_75Hz_32MHz);
...
// do all the drawing with calling these in any combination
VgaFB_Write(...), VgaFB_Read(...), VgaFB_DisplayEnabled(...), VgaFB_Clear(...), VgaFB_Scroll(...)
...
VgaFB_End(&v);

---------------------------------------------------------------------------------------
*/
#ifndef _VGAFB_CORE_H
#define _VGAFB_CORE_H

#include <stdint.h>
#include <SPI.h>


// ================== configurable part ==================

/**
 * Set it to how many address bytes the memory chip needs. It's usually 2 for <=64kB chips, but
 * can be 3 or 4 for larger chips. It also defines internal vram pointer type (uint16_t for 2)
 */
#define VGAFB_VRAM_ADDR_LENGTH				2

/**
 * Define how much video memory there is. There are two cases:
 * # if VgaFB_Scroll is used then this MUST be >= memory chip size
 *   (otherwise scrolling may move some uncleared pixels into blanking scanlines)
 * # if VgaFB_Scroll is not used then this MUST be >= VRAM used (vga mode vTotal*hTotal).
 *   Then the upper memory region can be used for user data
 */
#define VGAFB_VRAM_SIZE 65536

/**
 * Define how many data bytes are read or written in one transaction. If it's too large
 * then it will hold back vSync ISR and screen will ficker. Low values mean slow memory
 * access. 8 is good for Atmega328 running at 16MHz and using 8MHz SPI clock
 */
#define VGAFB_MAX_SPI_TRANSACTION_BYTES		8

/**
 * Comment this block in to configure 2 pins for ISR and SPI transaction timing measurements.
 * These are Atmega 328 PORTD bit numbers (0..7), not Arduino pin numbers
 */
//#define VGAFB_DEBUG
//#define VGAFB_DEBUG_PIN_TRANSACTION	6
//#define VGAFB_DEBUG_PIN_ISR			2

// ================== /configurable part ==================


// Defines pins that VgaFB uses and that can't be changed (arduino pin number)
#define VGAFB_FIXED_PIN_HSYNC		5
#define VGAFB_FIXED_PIN_VSYNC		10

#if VGAFB_VRAM_ADDR_LENGTH == 2
#define uint_vgafb_t uint16_t
#elif VGAFB_VRAM_ADDR_LENGTH == 3
// if there was uint24_t then we'd use it
#define uint_vgafb_t uint32_t
#elif VGAFB_VRAM_ADDR_LENGTH == 4
#define uint_vgafb_t uint32_t
#else
#error VGAFB_VRAM_ADDR_LENGTH must be 2, 3 or 4
#endif

/**
 * Display mode flags. Can be bitwise ORed.
 */
enum vgaflags_t {
	VGAFLAG_NONE = 0,
	VGAFLAG_INVERTED_VSYNC = 1,
	VGAFLAG_INVERTED_HSYNC = 2
};

/**
 * Display mode parameters. This is essentially XFree86 Modeline.
 *
 * A prefefined mode in vgafb_modes.cpp may look like "vgamode_320x240_75Hz_16MHz".
 * This means 320x240 visible pixels, vertical refresh at 75Hz and requires 16MHz pixel clock.
 */
typedef struct {
	uint16_t hVisible, hSyncStart, hSyncEnd, hTotal;
	uint16_t vVisible, vSyncStart, vSyncEnd, vTotal;
	uint8_t scanlineHeight; // vmemScaledStride = vmemStride * scanlineHeight
	vgaflags_t flags;
} vgamode_t;

/**
 * Internal state of VgaFB.
 *
 * Don't modify it directly, but read fields as neccesary (for VRAM addess calculations).
 */
typedef struct {
	vgamode_t mode;
	bool enabled; // if false then pixels are not clocked out and MCU has exclusive VRAM access
	
	uint16_t vVisibleScaled;
	uint_vgafb_t vmemPtr;
	uint_vgafb_t vmemFirstPixelOffset;
	uint_vgafb_t vmemLastPixelOffset; // where pixel is 1
	uint8_t vmemStride; // must be /2 (for clearScanline)
	uint8_t vmemScaledStride;
	uint16_t vSyncTimerIncCount; // how many 8-pixel blocks (==TCNT1 steps) does one vsync pulse

	uint8_t pxclk_mul, pxclk_div; // keep pxclk_div<=4 or there may be overflow in VgaFB_WaitAndStart
	volatile uint8_t* ab_port; // = (volatile uint8_t*)&PORTB;
	uint8_t ab_pin_mask; // = 0b00000010;
	volatile uint8_t* cs_port;
	uint8_t cs_pin_mask;

	SPISettings sramSpiSettings;
} vgafb_t;


/**
 * Configures MCU, tells how the hardware is wired up.
 *
 *   mul/div: Choose lowest integers that relate generated pixel clock to CPU clock.
 *            For example, if CPU runs at 16MHz and pxclk is 20MHz then this would be 5/4
 *   cs_pin:  Arduino pin number that is connected to /SEL (memory chip select pin)
 *   ab_pin:  Arduino pin number that connects to either A or B. Which exactly depends on vgamode
 *
 * For a mode to be usable with pixel clock mul/div the following divisions must be whole numbers:
 *   (hTotal * div) / (8 * mul)
 *   ((hSyncEnd - hSyncStart) * div) / (8 * mul)
 */
void VgaFB_ConfigBoard(vgafb_t* vgafb, uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin);

/**
 * Clears video RAM, sets video mode and starts sending picture to display.
 *
 *   mode: choose one from vgafb_modes.cpp or define your own that is compatible
 *         with the generated pixel clock.
 */
bool VgaFB_Begin(vgafb_t* vgafb, vgamode_t mode);

/**
 * Stops sending picture to display. After that VgaFB_Begin with different mode can be called.
 */
void VgaFB_End(vgafb_t* vgafb);

/**
 * Call it to temporarily disable and then re-enable display.
 *
 * VRAM bandwidth is shared between CPU and display and display always has priority.
 * If there are white pixels on bottom part of the display then available bandwidth
 * for CPU is drastically reduced (to as low as about 0.5kB/s). Temporarily disabling
 * display makes all VRAM bandwidth available for MCU (maybe 0.5MB/s). If display
 * is disabled then entire VRAM can be accessed using Arduino SPI calls and VgaFB_Read
 * and VgaFB_Write, but be careful what you write there. If in doubt overwrite
 * temporarily used VRAM with zeros afterwards, it's safe.
 */
void VgaFB_DisplayEnabled(vgafb_t *vgafb, bool on);

/**
 * Clears display to black
 */
void VgaFB_Clear(vgafb_t* vgafb);

/**
 * Scrolls screen up or down. New lines will be blank.
 *
 * Scrolling doesn't copy data in VRAM, but updates scroll pointer that is automatically
 * added to src or dst in VgaFB_Write and VgaFB_Read.
 */
void VgaFB_Scroll(vgafb_t* vgafb, int16_t delta);

/**
 * Writes to VRAM
 *
 * VRAM memory layout has vertical and horizontal blanking bytes in it. Keep them zero
 * or display may act funny (flicker, go blank, messed up brightness).
 * VRAM offset for pixel (x,y) is:
 *   uint_vgafb_t pixel_offset = vgafb->vmemFirstPixelOffset + x + y * vgafb->vmemScaledStride; 
 * where x=0..320/8-1 and y=0..240-1
 *
 *   dst: VRAM offset (pointer)
 *   buf: local buffer
 *   cnt: byte count to be written
 */
void VgaFB_Write(vgafb_t* vgafb, uint_vgafb_t dst, uint8_t* buf, uint_vgafb_t cnt);

/**
 * Reads from VRAM
 *
 * See VgaFB_Write how to calculate pixel offsets.
 *
 *   dst: VRAM offset (pointer)
 *   buf: local buffer
 *   cnt: byte count to be read
 */
void VgaFB_Read(vgafb_t* vgafb, uint_vgafb_t src, uint8_t* buf, uint_vgafb_t cnt);

/**
 * Sets one pixel color (slow)
 *
 *  x, y:   coordinates (it's safe for these to be out of range)
 *  pixel:  0 is black, anything else is white
 */
void VgaFB_SetPixel(vgafb_t* vgafb, uint16_t x, uint16_t y, uint8_t pixel);

/**
 * Gets one pixel color (slow)
 *
 *  x, y:   coordinates (it's safe for these to be out of range)
 *
 *  return: 0 if black, 1 if white. 0 if coordinates are out of range
 */
uint8_t VgaFB_GetPixel(vgafb_t* vgafb, uint16_t x, uint16_t y);


/**
 * Predefined display modes. For details see vgafb_modes.cpp
 *
 * For each mode VRAM used is calculated here. Memory chip must be at least that big
 */
extern vgamode_t vgamode_400x300_60Hz_20MHz;   // 41291 bytes
extern vgamode_t vgamode_256x256_60Hz_20MHz;   // 41291 bytes
extern vgamode_t vgamode_640x480_75Hz_32MHz;   // 53000 bytes
extern vgamode_t vgamode_320x240_75Hz_16MHz;   // 26500 bytes
extern vgamode_t vgamode_640x400_85Hz_32MHz;   // 46280 btyes
extern vgamode_t vgamode_320x200_85Hz_16MHz;   // 23140 bytes
extern vgamode_t vgamode_640x480_60Hz_25MHz;   // 52500 bytes
extern vgamode_t vgamode_320x240_60Hz_12_6MHz; // 26250 bytes
extern vgamode_t vgamode_160x120_60Hz_6_3MHz;  // 13125 bytes

#endif
