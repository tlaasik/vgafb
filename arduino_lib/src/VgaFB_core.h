#ifndef _VGAFB_CORE_H
#define _VGAFB_CORE_H

#include <Arduino.h>
#include <SPI.h>

// ===== configurable part ======

// Commenting this in will configure 2 debug pins for ISR and SPI transaction timing measurements
//#define VGAFB_DEBUG

// Lower for slower SPI speeds, increase for higher speeds. 8 is good for 8MHz SPI clock
#define VGAFB_MAX_SPI_TRANSACTION_BYTES		8
#define VGAFB_MAX_SPI_TRANSACTION_WORDS		(VGAFB_MAX_SPI_TRANSACTION_BYTES / 2)

// Set it to how many address bytes memory chip needs. It's usually 2 or 3
#define VGAFB_VRAM_ADDR_LENGTH 2

// Define how much video memory there is. There are two cases:
// * if VgaFB_Scroll is used then this MUST be >= memory chip size
//   (otherwise scrolling may move some uncleared pixels into blanking scanlines)
// * if VgaFB_Scroll is not used then this MUST be >= VRAM used (mode vTotal*hTotal).
//   Then the upper memory region can be used for user data
#define VGAFB_VRAM_SIZE 65536

// ===== /configurable part ======

#if VGAFB_VRAM_ADDR_LENGTH != 2 && VGAFB_VRAM_ADDR_LENGTH != 3
#error VGAFB_VRAM_ADDR_LENGTH must be 2 or 3
#endif

// TODO use better name
// vgamode_t flags
#define VGA_INVERTED_VSYNC	1
#define VGA_INVERTED_HSYNC	2

#define SET_PORT_PIN(port, pin_mask) (*(port) |= (pin_mask))
#define CLR_PORT_PIN(port, pin_mask) (*(port) &= ~(pin_mask))
#define VGAFB_START_CRIT() uint8_t _sreg = SREG; noInterrupts()
#define VGAFB_END_CRIT() SREG = _sreg

// TODO these shouldn't be here?
#define hSync		5
#define vSync		10

// TODO this MUST have the same value as U8X8_PIN_NONE. do smth about it
#define VGAFB_PIN_NONE 255

#ifdef VGAFB_DEBUG
// TODO move these two pin numbers out from here?
#define dbgPin		6
#define dbgPinInt	2
#define VGAFB_DEBUG_CLR(x,y) (x&=(~(1<<y)))
#define VGAFB_DEBUG_SET(x,y) (x|=(1<<y))
#else
#define VGAFB_DEBUG_CLR(x,y)
#define VGAFB_DEBUG_SET(x,y)
#endif

#if VGAFB_VRAM_ADDR_LENGTH <= 2
#define uint_vgafb_t uint16_t
#else
#define uint_vgafb_t uint32_t
#endif


typedef struct {
	uint16_t hVisible, hSyncStart, hSyncEnd, hTotal;
	uint16_t vVisible, vSyncStart, vSyncEnd, vTotal;
	uint8_t scanlineHeight; // vmemScaledStride = vmemStride * scanlineHeight
	uint8_t flags;
} vgamode_t;

typedef struct {
	vgamode_t mode;
	
	uint16_t vVisibleScaled;
	uint_vgafb_t vmemPtr;
	uint_vgafb_t vmemFirstPixelOffset;
	uint_vgafb_t vmemLastPixelOffset; // where pixel is 1
	uint8_t vmemStride; // must be /2 (for clearScanline)
	uint8_t vmemScaledStride;
	uint16_t vSyncTimerIncCount; // how many 8-pixel blocks (==TCNT1 steps) does one vsync pulse

	uint8_t pxclk_mul, pxclk_div; // keep pxclk_div<=4 or there may be overflow in VgaFB_WaitAndStart
	volatile uint8_t* ab_port;// = (volatile uint8_t*)&PORTB;
	uint8_t ab_pin_mask;// = 0b00000010;
	volatile uint8_t* cs_port;
	uint8_t cs_pin_mask;

	SPISettings sramSpiSettings;
} vgafb_t;


// VGA native modes
extern vgamode_t vgamode_640x480_75Hz_32MHz;
extern vgamode_t vgamode_640x400_85Hz_32MHz;
// VGA doublescan modes
extern vgamode_t vgamode_400x300_60Hz_20MHz;
extern vgamode_t vgamode_320x240_75Hz_16MHz;
extern vgamode_t vgamode_320x200_85Hz_16MHz;
// VGA partial screen doublescan modes
extern vgamode_t vgamode_256x256_60Hz_20MHz; // based on 400x300


void VgaFB_ConfigBoard(vgafb_t* vgafb, uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin);
bool VgaFB_Begin(vgafb_t* vgafb, vgamode_t mode);
void VgaFB_End(vgafb_t* vgafb);

void VgaFB_Clear(vgafb_t* vgafb);

void VgaFB_Scroll(vgafb_t* vgafb, int16_t delta);

void VgaFB_Write(vgafb_t* vgafb, uint_vgafb_t dst, uint8_t* buf, uint8_t cnt);
void VgaFB_Read(vgafb_t* vgafb, uint_vgafb_t src, uint8_t* buf, uint8_t cnt);

#endif
