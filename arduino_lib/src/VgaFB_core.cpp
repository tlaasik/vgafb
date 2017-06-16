/*

VgaFB_core.cpp

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
*/
#include "stdint.h"
#include <SPI.h>
#include "VgaFB_core.h"
//not using TimerHelpers.h by Nick Gammon any more (http://www.gammon.com.au/forum/?id=11504)


#if VGAFB_VRAM_ADDR_LENGTH == 2
#define SPI_SEND_CMD_AND_ADDR(cmd, addr) \
	{ SPI.transfer(cmd); SPI.transfer16((uint16_t)addr); }
#elif VGAFB_VRAM_ADDR_LENGTH == 3
#define SPI_SEND_CMD_AND_ADDR(uint8_t cmd, uint_vgafb_t addr) \
	{ SPI.transfer(cmd); SPI.transfer((uint16_t)(addr >> 16)); SPI.transfer16((uint16_t)addr); }
#elif VGAFB_VRAM_ADDR_LENGTH == 4
#define SPI_SEND_CMD_AND_ADDR(uint8_t cmd, uint_vgafb_t addr) \
	{ SPI.transfer(cmd); SPI.transfer16((uint16_t)(addr >> 16)); SPI.transfer16((uint16_t)addr); }
#endif

#define SET_PORT_PIN(port, pin_mask) (*(port) |= (pin_mask))
#define CLR_PORT_PIN(port, pin_mask) (*(port) &= ~(pin_mask))
#define VGAFB_START_CRIT() uint8_t _sreg = SREG; noInterrupts()
#define VGAFB_END_CRIT() SREG = _sreg

#ifdef VGAFB_DEBUG
#define VGAFB_DEBUG_CLR(x,y) (x&=(~(1<<y)))
#define VGAFB_DEBUG_SET(x,y) (x|=(1<<y))
#else
#define VGAFB_DEBUG_CLR(x,y)
#define VGAFB_DEBUG_SET(x,y)
#endif

static vgafb_t* cur_vgafb = 0;

ISR(TIMER1_OVF_vect)
{
	if (cur_vgafb == 0 || !cur_vgafb->enabled) // not initialized or not enabled
		return;

	VGAFB_DEBUG_SET(PORTD, VGAFB_DEBUG_PIN_ISR);

	// if we're still in PXOUT mode then get out from it
	// prepare for clocking pixels out (send address)
	SPI.beginTransaction(cur_vgafb->sramSpiSettings);

	//_delay_us(11);

	// set mode to SPI and force any ongoing transfer to end by setting /CS=1
	// (most likely we already were in this state, but we need to be sure here)
	SET_PORT_PIN(cur_vgafb->cs_port, cur_vgafb->cs_pin_mask);
	SET_PORT_PIN(cur_vgafb->ab_port, cur_vgafb->ab_pin_mask);

	// start READ cmd
	CLR_PORT_PIN(cur_vgafb->cs_port, cur_vgafb->cs_pin_mask);
	SPI_SEND_CMD_AND_ADDR(0x03, cur_vgafb->vmemPtr); // READ

	// set mode to PXOUT. Pixels start clocking out after vsync edge.
	// this disconnects video hw from SPI so we can end transaction
	CLR_PORT_PIN(cur_vgafb->ab_port, cur_vgafb->ab_pin_mask);

	SPI.endTransaction();

	VGAFB_DEBUG_CLR(PORTD, VGAFB_DEBUG_PIN_ISR);
}


static void VgaFB_WaitAndStart(vgafb_t* vgafb)
{
	// wait until all white pixels are clocked out
	// (those << and >> ensure that there's no uint16 overflow by sacrificing precision by 2 bits)
	uint16_t v = (((vgafb->vmemLastPixelOffset + vgafb->vSyncTimerIncCount) >> 2) * vgafb->pxclk_div / vgafb->pxclk_mul) << 2;
	while (TCNT1 < v);
	
	// take exclusive access to SPI bus
	SPI.beginTransaction(vgafb->sramSpiSettings);
}

static void VgaFB_StartTranscation(vgafb_t* vgafb)
{
	// if it's enabled we need to wait until the last white pixel has been clocked out to display
	if (vgafb->enabled)
	{
		// (need this later)
		uint16_t tcntValueBeforeWait = TCNT1;

		VgaFB_WaitAndStart(vgafb);

		// Between "while" and "SPI.beginTransaction" Timer1 overflowed (rare, but occurs).
		// This means that we're holding back Timer1 ISR that needs to run NOW
		// to start clocking out next frame. By ending SPI transaction Timer1 ISR
		// will enter immediately and we will have to wait until the last white pixel
		// have been clocked out.
		if (TCNT1 < tcntValueBeforeWait)
		{
			SPI.endTransaction();

			VgaFB_WaitAndStart(vgafb);

			// Theoretically Timer1 may have overflown once again, but we can ignore that
			// because if that's the case then:
			// * there's no VGA Front Porch or in there are white pixels. Either way
			//   this condition mustn't appear during normal operation
			// * there's some other ISR that runs too long (no ISR mustn't take longer
			//   than one scanline time that's roughly ~50us, depending on vga mode)
			// * even if overflow happened we recover from it gracefully. One frame
			//   is skipped (will draw black)
		}
	}
	else {
		SPI.beginTransaction(vgafb->sramSpiSettings);
	}

	// ! we're in time critical section now until _endSpiTransaction is called
	// (interrupts were disabled in SPI.beginTransaction)

	VGAFB_DEBUG_SET(PORTD, VGAFB_DEBUG_PIN_TRANSACTION);

	// exit PXOUT mode and deselect vram /CS=1
	// (most likely we already were in this state, but we need to be sure here)
	SET_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);
	SET_PORT_PIN(vgafb->ab_port, vgafb->ab_pin_mask);
	// !cpu is way too slow for this. natural delay is enough! u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
	CLR_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);
}

static void VgaFB_EndTransaction(vgafb_t* vgafb)
{
	// !cpu is way too slow for this, natural delay is enough! u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
	SET_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);
	VGAFB_DEBUG_CLR(PORTD, VGAFB_DEBUG_PIN_TRANSACTION);
	SPI.endTransaction();
}

void VgaFB_ConfigBoard(vgafb_t* vgafb, uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin)
{
	uint8_t cs_port = digitalPinToPort(cs_pin);
	uint8_t ab_port = digitalPinToPort(ab_pin);
	if (cs_port == NOT_A_PIN || ab_port == NOT_A_PIN)
		return; // XXX this is fail, but there's no way to indicate it right now

	vgafb->cs_port = portOutputRegister(cs_port);
	vgafb->ab_port = portOutputRegister(ab_port);
	vgafb->cs_pin_mask = digitalPinToBitMask(cs_pin);
	vgafb->ab_pin_mask = digitalPinToBitMask(ab_pin);

	pinMode(VGAFB_FIXED_PIN_HSYNC, OUTPUT);
	pinMode(VGAFB_FIXED_PIN_VSYNC, OUTPUT);
	pinMode(ab_pin, OUTPUT);
	pinMode(cs_pin, OUTPUT);

	vgafb->pxclk_mul = mul;
	vgafb->pxclk_div = div;

#ifdef VGAFB_DEBUG
	pinMode(VGAFB_DEBUG_PIN_TRANSACTION, OUTPUT);
	pinMode(VGAFB_DEBUG_PIN_ISR, OUTPUT);
#endif
}

bool VgaFB_Begin(vgafb_t* vgafb, vgamode_t mode)
{
	// argument sanity checks
	if (cur_vgafb != 0)
		return false;
	if (mode.hVisible >= mode.hSyncStart || mode.hSyncStart >= mode.hSyncEnd || mode.hSyncEnd >= mode.hTotal || mode.hTotal <= 0)
		return false;
	if (mode.vVisible >= mode.vSyncStart || mode.vSyncStart >= mode.vSyncEnd || mode.vSyncEnd >= mode.vTotal || mode.vTotal <= 0)
		return false;

	// total line width must be dividable by 8 due to prescaler
	//if ((hTotal & 0x07) != 0)
	//	return false;

	vgafb->mode = mode;
	
	vgafb->vVisibleScaled = mode.vVisible / vgafb->mode.scanlineHeight;
	vgafb->vmemPtr = 0;
	vgafb->vmemStride = mode.hTotal >> 3;
	vgafb->vmemScaledStride = vgafb->vmemStride * mode.scanlineHeight;
	vgafb->vmemFirstPixelOffset = (mode.vTotal - mode.vSyncEnd) * vgafb->vmemStride + ((mode.hTotal - mode.hSyncStart) >> 3);

	uint8_t hLineTime = (mode.hTotal * vgafb->pxclk_div / vgafb->pxclk_mul) >> 3; // +1=+0.5uS (16MHz pclk)
	uint8_t hSyncPulse = ((mode.hSyncEnd - mode.hSyncStart) * vgafb->pxclk_div / vgafb->pxclk_mul) >> 3; // +1=+0.5uS (16MHz pclk)
	uint8_t vSyncPulse = mode.vSyncEnd - mode.vSyncStart;

	vgafb->vSyncTimerIncCount = vSyncPulse * vgafb->vmemStride;

	SET_PORT_PIN(vgafb->ab_port, vgafb->ab_pin_mask);
	
	//vgafb->sramSpiSettings = SPISettings(u8x8->display_info->sck_clock_hz, MSBFIRST, internal_spi_mode);
	vgafb->sramSpiSettings = SPISettings(20000000, MSBFIRST, SPI_MODE0);

	// clear all SRAM (smaller chips will be cleared multiple times, but it doesn't break anything)
	vgafb->enabled = false;
	VgaFB_Write(vgafb, 0, 0, VGAFB_VRAM_SIZE - 1);
	VgaFB_Write(vgafb, VGAFB_VRAM_SIZE - 1, 0, 1);
	
	VGAFB_START_CRIT();

	// reset timer flags and values
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT0 = 0;
	TCNT1 = 0;

	// calculate values for proper timer frequency and duty cycle
	OCR0A = hLineTime - 1;		// whole line time. +1 step = +0.5uS (16MHz pclk)
	OCR0B = hSyncPulse - 1;		// +1 = +0.5us(16MHz pclk)
	OCR1A = (uint16_t)hLineTime * (uint16_t)mode.vTotal - 1;
	OCR1B = (uint16_t)hLineTime * (uint16_t)vSyncPulse - 1;

	// start the timers
	////Timer0 mode 7: Fast PWM, top = OCR0A
	//Timer0::setMode(7, Timer0::PRESCALE_8, mode.flags & VGAFLAG_INVERTED_HSYNC
	//	? Timer0::SET_B_ON_COMPARE : Timer0::CLEAR_B_ON_COMPARE);
	////Timer1 mode 15: Fast PWM, TOP = OCR1A
	//Timer1::setMode(15, Timer1::PRESCALE_8, mode.flags & VGAFLAG_INVERTED_VSYNC
	//	? Timer1::SET_B_ON_COMPARE : Timer1::CLEAR_B_ON_COMPARE);
	uint8_t _0a = _BV(WGM00) | _BV(WGM01) | (mode.flags & VGAFLAG_INVERTED_HSYNC ? _BV(COM0B0) | _BV(COM0B1) : _BV(COM0B1));
	uint8_t _1a = _BV(WGM10) | _BV(WGM11) | (mode.flags & VGAFLAG_INVERTED_VSYNC ? _BV(COM1B0) | _BV(COM1B1) : _BV(COM1B1));
	GTCCR |= _BV(PSRASY); // reset prescaler by setting a bit right before starting the timers
	TCCR0A = _0a;
	TCCR1A = _1a;
	TCCR0B = _BV(WGM02) | 2;
	TCCR1B = _BV(WGM12) | _BV(WGM13) | 2;
	
	TIFR1 = _BV(TOV1);		// clear overflow flag
	TIMSK1 = _BV(TOIE1);	// interrupt on Timer0 overflow

	cur_vgafb = vgafb;

	vgafb->enabled = true;
	VGAFB_END_CRIT();
}

void VgaFB_End(vgafb_t* vgafb)
{
	VGAFB_START_CRIT();
	
	// stop timers
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR1A = 0;
	TCCR1B = 0;

	cur_vgafb = 0;

	VGAFB_END_CRIT();
}

void VgaFB_DisplayEnabled(vgafb_t *vgafb, bool enabled)
{
	vgafb->enabled = enabled;
}

void VgaFB_Clear(vgafb_t* vgafb)
{
	VgaFB_StartTranscation(vgafb);
	SPI_SEND_CMD_AND_ADDR(0x02, vgafb->vmemPtr); // WRITE
	uint_vgafb_t wordsToErase = (vgafb->vmemLastPixelOffset >> 1) + 1;// vgafb->vTotal * vgafb->vmemStride / 2;
	while (wordsToErase--)
		SPI.transfer16(0);
	SET_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);

	vgafb->vmemLastPixelOffset = 0;

	// We're about to end a very long SPI transaction that has grossly violated
	// maximum time we are allowed to hold back Timer1 overflow ISR. In fact
	// Timer1 has overflown multiple times. In this case this is all OK, because
	// while we were holding Timer1 ISR back (that starts clocking pixels out)
	// the screen was displaying black and the end result of calling this function
	// is that all pixels in VRAM are black too. To cleanly end this SPI transaction
	// Timer1 overflow flag must be cleared.
	TIFR1 = _BV(TOV1);

	VgaFB_EndTransaction(vgafb);
}

// scanline vTotal (not scaled line vTotalScaled)
// including lines in blanking area and offscreen (<0 and >vTotal)
static void VgaFB_ClearScanline(vgafb_t *vgafb, int16_t scanline)
{
	uint_vgafb_t offset = vgafb->vmemFirstPixelOffset + scanline * vgafb->vmemStride;
	VgaFB_Write(vgafb, offset, 0, vgafb->vmemStride);
}

void VgaFB_Scroll(vgafb_t* vgafb, int16_t delta)
{
	delta *= vgafb->mode.scanlineHeight;

	// scroll up
	while (delta > 0)
	{
		//VgaFB_ClearScanline(vgafb, vgafb->mode.vTotal); // new blanking line
		VgaFB_ClearScanline(vgafb, 0);
		uint8_t sreg = SREG;
		noInterrupts();
		vgafb->vmemPtr += vgafb->vmemStride;
		SREG = sreg; // restore interrupts_enabled state that was before noInterrupts()
		if (vgafb->vmemLastPixelOffset >= vgafb->vmemStride)
			vgafb->vmemLastPixelOffset -= vgafb->vmemStride;
		delta--;
	}
	// scroll down
	while (delta < 0) {
		uint16_t lastLineOffset = vgafb->mode.vVisible - 1;
		//VgaFB_ClearScanline(vgafb, -1); // new blanking line
		VgaFB_ClearScanline(vgafb, lastLineOffset); // last visible line becames blanking line
		uint8_t sreg = SREG;
		noInterrupts();
		vgafb->vmemPtr -= vgafb->vmemStride;
		SREG = sreg; // restore interrupts_enabled state that was before noInterrupts()
		vgafb->vmemLastPixelOffset += vgafb->vmemStride;
		// XXX the first -1 on next line may cause hiding the last line while screen is updating
		//     when not using DOUBLESCAN. When using DOUBLESCAN the last line may be cut to half
		//     which is probably ok because of the performance benefits (2x). If testing shows
		//     thet this -1 does not have this sideeffect, then this comment should be removed
		//     and an explanation given why this is so
		uint_vgafb_t lastPossiblyVisibleOffset = lastLineOffset * vgafb->vmemStride - 1;
		if (vgafb->vmemLastPixelOffset > lastPossiblyVisibleOffset)
			vgafb->vmemLastPixelOffset = lastPossiblyVisibleOffset;
		delta++;
	}
}

// special case: if buf is null (0) then zeros are written
void VgaFB_Write(vgafb_t* vgafb, uint_vgafb_t dst, uint8_t* buf, uint_vgafb_t cnt)
{
	bool mayPushLastPixel = buf != 0 && dst + cnt > vgafb->vmemLastPixelOffset;

	uint8_t b[VGAFB_MAX_SPI_TRANSACTION_BYTES];
	while (cnt > 0)
	{
		uint8_t c = cnt > VGAFB_MAX_SPI_TRANSACTION_BYTES ? VGAFB_MAX_SPI_TRANSACTION_BYTES : cnt;

		uint8_t i = c;
		if (buf) {
			while (i--)
				b[i] = buf[i];
		} else {
			while (i--)
				b[i] = 0;
		}
		
		VgaFB_StartTranscation(vgafb);
		SPI_SEND_CMD_AND_ADDR(0x02, vgafb->vmemPtr + dst); // WRITE
		SPI.transfer(b, c);
		VgaFB_EndTransaction(vgafb);

		if (mayPushLastPixel) {
			i = c;
			while (i--) {
				if (buf[i] != 0 && dst + i > vgafb->vmemLastPixelOffset) {
					vgafb->vmemLastPixelOffset = dst + i;
					break;
				}
			}
		}

		cnt -= c;
		dst += c;
		if(buf)
			buf += c;
	}
}

void VgaFB_Read(vgafb_t* vgafb, uint_vgafb_t src, uint8_t* buf, uint_vgafb_t cnt)
{
	uint_vgafb_t addr = vgafb->vmemPtr + src;
	while(cnt > 0)
	{
		uint8_t c = cnt > VGAFB_MAX_SPI_TRANSACTION_BYTES ? VGAFB_MAX_SPI_TRANSACTION_BYTES : cnt;

		VgaFB_StartTranscation(vgafb);
		SPI_SEND_CMD_AND_ADDR(0x03, addr); // READ
		SPI.transfer(buf, c);
		VgaFB_EndTransaction(vgafb);

		cnt -= c;
		addr += c;
	}
}


void VgaFB_SetPixel(vgafb_t* vgafb, uint16_t x, uint16_t y, uint8_t pixel)
{
	if (y >= vgafb->vVisibleScaled || x >= vgafb->mode.hVisible)
		return;

	uint_vgafb_t offset = vgafb->vmemFirstPixelOffset + y * vgafb->vmemScaledStride + ((uint16_t)x >> 3);
	uint8_t bitOffset = 7 - (x & 0x07);
	uint8_t mask = 1 << bitOffset;

	uint8_t read;
	VgaFB_Read(vgafb, offset, &read, 1);
	uint8_t write = (read & ~mask) | ((pixel ? 0xFF : 0) & mask);

	uint8_t s = vgafb->mode.scanlineHeight;
	while (s--) {
		VgaFB_Write(vgafb, offset, &write, 1);
		offset += vgafb->vmemStride;
	}
}
uint8_t VgaFB_GetPixel(vgafb_t* vgafb, uint16_t x, uint16_t y)
{
	if (y >= vgafb->vVisibleScaled || x >= vgafb->mode.hVisible)
		return 0;

	uint_vgafb_t offset = vgafb->vmemFirstPixelOffset + y * vgafb->vmemScaledStride + ((uint16_t)x >> 3);

	uint8_t read;
	VgaFB_Read(vgafb, offset, &read, 1);

	return (read & (0x080 >> (x & 0x07))) ? 1 : 0;
}
