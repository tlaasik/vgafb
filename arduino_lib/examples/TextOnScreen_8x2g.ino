
#include <SPI.h>
#include <U8x8lib.h>
#include <VgaFB_core.h>
#include <VgaFB_u8g2.h>

U8G2_VGAFB_400X300_60Hz_20MHz_1 u8g2(U8G2_R0, /*mul/div=*/5, 4, /* cs=*/ 4, /* a=*/ 9);

// Generate 8MHz that will be multiplied by 2.5 (set by jumpers) to get 20MHz pixel clock.
// Since Arduino itself runs at 16MHz we told in u8g2 constructor that the multiplier is 1.25
void setupClockGen() {
	OCR2A = 0;
	TCCR2A = _BV(WGM21) | _BV(COM2B0); // toggle B on compare (pin 5, Arduino port: D3)
	TCCR2B = 1; // prescale 1
	pinMode(3, OUTPUT);
}

void setup() {
	setupClockGen();
	u8g2.begin();
}
void loop()
{
	u8g2.firstPage();
	do {
		u8g2.setFont(u8g2_font_ncenB14_tr);
		u8g2.drawStr(0, 20, "VGA framebuffer for Arduino Nano");
		u8g2.drawStr(130, 260, "Running at 400x300 60Hz");
		u8g2.setFont(u8g2_font_ncenB08_tr);
		u8g2.drawStr(130, 280, "Using u8g2 for rendering text");
		u8g2.drawStr(0, 40, "https://github.com/tlaasik/vgafb");
		u8g2.drawFilledEllipse(100, 100, 90, 30);
	} while (u8g2.nextPage());

	_delay_ms(4000);
	VgaFB_Scroll(u8g2.getVgaFB(), 300);
	_delay_ms(800);
}
