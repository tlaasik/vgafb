
#include "stdint.h"
#include "VgaFB_core.h"
#include <TimerHelpers.h>
#include <SPI.h>

static vgafb_t* cur_vgafb = 0;

ISR(TIMER1_OVF_vect)
{
	if (cur_vgafb == 0) // not initialized
		return;

	VGAFB_DEBUG_SET(PORTD, dbgPinInt);

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
	SPI.transfer(0x03); // READ
	SPI.transfer16(cur_vgafb->vmemPtr);

	// set mode to PXOUT. Pixels start clocking out after vsync edge.
	// this disconnects video hw from SPI so we can end transaction
	CLR_PORT_PIN(cur_vgafb->ab_port, cur_vgafb->ab_pin_mask);

	SPI.endTransaction();

	VGAFB_DEBUG_CLR(PORTD, dbgPinInt);
}


static void VgaFB_WaitAndStart(vgafb_t* vgafb)
{
	// wait until all white pixels are clocked out
	// (those << and >> ensure that there's no uint16 overflow by sacrificing precision by 2 bits)
	while (TCNT1 < (((vgafb->vmemLastPixelOffset + vgafb->vSyncTimerIncCount) >> 2) * vgafb->pxclk_div / vgafb->pxclk_mul) << 2);
	// take exclusive access to SPI bus
	SPI.beginTransaction(vgafb->sramSpiSettings);
}

void VgaFB_StartTranscation(vgafb_t* vgafb)
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

	// ! we're in time critical section now until _endSpiTransaction is called
	// (interrupts were disabled in SPI.beginTransaction)

	VGAFB_DEBUG_SET(PORTD, dbgPin); // just for debugging how long a transaction takes

	// exit PXOUT mode and deselect vram /CS=1
	// (most likely we already were in this state, but we need to be sure here)
	SET_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);
	SET_PORT_PIN(vgafb->ab_port, vgafb->ab_pin_mask);
	// !cpu is way too slow for this. natural delay is enough! u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
	CLR_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);
}

void VgaFB_EndTransaction(vgafb_t* vgafb)
{
	// !cpu is way too slow for this, natural delay is enough! u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
	SET_PORT_PIN(vgafb->cs_port, vgafb->cs_pin_mask);
	VGAFB_DEBUG_CLR(PORTD, dbgPin); // just for debugging how long a transaction takes
	SPI.endTransaction();
}

void VgaFB_ConfigBoard(vgafb_t* vgafb, uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t ab_pin)
{
	vgafb->pxclk_mul = mul;
	vgafb->pxclk_div = div;

	uint8_t port = digitalPinToPort(cs_pin);
	if (port == NOT_A_PIN)
		return; // XXX this is fail, but there's no way to indicate it right now
	vgafb->cs_port = portOutputRegister(port);
	vgafb->cs_pin_mask = digitalPinToBitMask(cs_pin);

	port = digitalPinToPort(ab_pin);
	if (port == NOT_A_PIN)
		return; // XXX this is fail, but there's no way to indicate it right now
	vgafb->ab_port = portOutputRegister(port);
	vgafb->ab_pin_mask = digitalPinToBitMask(ab_pin);
	
#ifdef VGAFB_DEBUG
	pinMode(dbgPin, OUTPUT);
	pinMode(dbgPinInt, OUTPUT);
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
	//vgafb->vmemScaledStride = mode.flags & VGA_DOUBLESCAN ? vgafb->vmemStride << 1 : vgafb->vmemStride;
	vgafb->vmemScaledStride = vgafb->vmemStride * mode.scanlineHeight;
	vgafb->vmemFirstPixelOffset = (mode.vTotal - mode.vSyncEnd) * vgafb->vmemStride + ((mode.hTotal - mode.hSyncStart) >> 3);

	uint8_t hLineTime = (mode.hTotal * vgafb->pxclk_div / vgafb->pxclk_mul) >> 3; // +1=+0.5uS (16MHz pclk)
	uint8_t hSyncPulse = ((mode.hSyncEnd - mode.hSyncStart) * vgafb->pxclk_div / vgafb->pxclk_mul) >> 3; // +1=+0.5uS (16MHz pclk)
	uint8_t vSyncPulse = mode.vSyncEnd - mode.vSyncStart;

	vgafb->vSyncTimerIncCount = vSyncPulse * vgafb->vmemStride;

	SET_PORT_PIN(vgafb->ab_port, vgafb->ab_pin_mask);
	
	//vgafb->sramSpiSettings = SPISettings(u8x8->display_info->sck_clock_hz, MSBFIRST, internal_spi_mode);
	vgafb->sramSpiSettings = SPISettings(20000000, MSBFIRST, SPI_MODE0);

	// clear all 64k of SRAM (smaller chips will be cleared multiple times, but it doesn't break anything)
	// TODO move initial ram clear into it's own function and it shouldn't use SPI HW directly
	TCNT1 = 0; // safety (otherwise there's a chance that u8x8_cad_StartTransfer gets stuck)
	VgaFB_StartTranscation(vgafb);
	SPI.transfer(0x02); // WRITE
	SPI.transfer16(0); // addr
	uint16_t wordsToErase = 65536 / 2;
	while (wordsToErase--)
		SPI.transfer16(0);
	VgaFB_EndTransaction(vgafb);

	// critical block starts here
	uint8_t sreg = SREG;
	noInterrupts();

	Timer0::setMode(7, Timer0::PRESCALE_8, mode.flags & VGA_INVERTED_HSYNC
		? Timer0::SET_B_ON_COMPARE : Timer0::CLEAR_B_ON_COMPARE);
	OCR0A = hLineTime - 1;        // whole line time. +1 step = +0.5uS (16MHz pclk)
	OCR0B = hSyncPulse - 1;			  // +1 = +0.5us(16MHz pclk)

	Timer1::setMode(15, Timer1::PRESCALE_8, mode.flags & VGA_INVERTED_VSYNC
		? Timer1::SET_B_ON_COMPARE : Timer1::CLEAR_B_ON_COMPARE);
	OCR1A = (uint16_t)hLineTime * (uint16_t)mode.vTotal - 1;
	OCR1B = (uint16_t)hLineTime * (uint16_t)vSyncPulse - 1;

	//TCNT0 = 0; // TODO sometimes commenting this in will fix hsync timing (there's one timer count lag sometimes)
	//TCNT1 = 0;
	GTCCR |= _BV(PSRASY); // reset prescaler by setting a bit
	TCNT0 = 0;
	TCNT1 = 0;

	TIFR1 = _BV(TOV1);		// clear overflow flag
	TIMSK1 = _BV(TOIE1);	// interrupt on Timer0 overflow

	SREG = sreg; // restore interrupts if they were enabled
	// critical block ends here. clkOut doesn't need to be in it, because it causes no interrupts

	bool _enableClkOut = true; // TODO
	if (_enableClkOut) {
		// tmp 8MHz pixel clock
		OCR2A = 0;
		Timer2::setMode(2, Timer2::PRESCALE_1, Timer2::TOGGLE_B_ON_COMPARE); // CTC, TOP=OCR2A
		pinMode(clkOut, OUTPUT);
	}
	cur_vgafb = vgafb;
}


void VgaFB_End(vgafb_t* vgafb) {
	// TODO
	cur_vgafb = 0;
}

void VgaFB_Clear(vgafb_t* vgafb)
{
	VgaFB_StartTranscation(vgafb);
	SPI.transfer(0x02); // WRITE
	SPI.transfer16(vgafb->vmemPtr);
	uint16_t wordsToErase = (vgafb->vmemLastPixelOffset >> 1) + 1;// vgafb->vTotal * vgafb->vmemStride / 2;
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
void VgaFB_ClearScanline(vgafb_t *vgafb, int16_t scanline)
{
	scanline += vgafb->mode.vTotal - vgafb->mode.vSyncEnd;

	int16_t offset = scanline * vgafb->vmemStride;

	int16_t wordCount = vgafb->vmemStride >> 1;
	while (wordCount > 0)
	{
		uint8_t w = wordCount > VGAFB_MAX_SPI_TRANSACTION_WORDS ? VGAFB_MAX_SPI_TRANSACTION_WORDS : (uint8_t)wordCount;

		VgaFB_StartTranscation(vgafb);
		SPI.transfer(0x02); // WRITE
		SPI.transfer16(vgafb->vmemPtr + offset);
		while (w--) SPI.transfer16(0);
		VgaFB_EndTransaction(vgafb);

		wordCount -= VGAFB_MAX_SPI_TRANSACTION_WORDS;
		offset += VGAFB_MAX_SPI_TRANSACTION_WORDS << 1;
	}
}

void VgaFB_Scroll(vgafb_t* vgafb, int16_t delta) // scanline
{
	delta *= vgafb->mode.scanlineHeight;

	// scroll up
	while (delta > 0)
	{
		//ClearLine(_vTotal); // new blanking line
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
		//ClearLine(-1); // new blanking line
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
		uint16_t lastPossiblyVisibleOffset = lastLineOffset * vgafb->vmemStride - 1;
		if (vgafb->vmemLastPixelOffset > lastPossiblyVisibleOffset)
			vgafb->vmemLastPixelOffset = lastPossiblyVisibleOffset;
		delta++;
	}
}

void VgaFB_Write(vgafb_t* vgafb, uint16_t dst, uint8_t* buf, uint8_t cnt)
{
	bool mayPushLastPixel = dst + cnt > vgafb->vmemLastPixelOffset;

	uint8_t b[VGAFB_MAX_SPI_TRANSACTION_BYTES];
	while (cnt > 0)
	{
		uint8_t c = cnt > VGAFB_MAX_SPI_TRANSACTION_BYTES ? VGAFB_MAX_SPI_TRANSACTION_BYTES : cnt;
		
		uint8_t i = c;
		while(i--)
			b[i] = buf[i];
		
		VgaFB_StartTranscation(vgafb);
		SPI.transfer(0x02); // WRITE
		SPI.transfer16(vgafb->vmemPtr + dst);
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
		buf += c;
	}
}

void VgaFB_Read(vgafb_t* vgafb, uint16_t src, uint8_t* buf, uint8_t cnt)
{
	uint16_t addr = vgafb->vmemPtr + src;
	while(cnt > 0)
	{
		uint8_t c = cnt > VGAFB_MAX_SPI_TRANSACTION_BYTES ? VGAFB_MAX_SPI_TRANSACTION_BYTES : cnt;

		VgaFB_StartTranscation(vgafb);
		SPI.transfer(0x03); // READ
		SPI.transfer16(addr);
		SPI.transfer(buf, c);
		VgaFB_EndTransaction(vgafb);

		cnt -= c;
		addr += c;
	}
}
