
#include <SPI.h>
#include <U8x8lib.h>
#include <VgaFB.h>
#include <VgaFB_u8g2.h>

#ifndef U8G2_16BIT
#error For this example U8G2_16BIT must be defined in "U8g2/src/clib/u8g2.h" (because we're using higher than 240x240 resolution here)
#endif

// Create class instances
U8G2_VGAFB_400X300_60Hz_20MHz_1 u8g2(U8G2_R0, /*mul/div=*/5, 4, /* cs=*/ 4, /* a=*/ 9);
VgaFB vgafb;

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
  vgafb.setVgaFB(u8g2.getVgaFB());
}
void loop()
{
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, "VGA framebuffer for Arduino Nano");
    u8g2.drawStr(130, 260, "Running on 2022 at 400x300 60Hz");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(130, 280, "Using u8g2 for rendering text");
    u8g2.drawStr(0, 40, "https://github.com/tlaasik/vgafb");
    u8g2.drawFilledEllipse(100, 100, 90, 30);
  } while (u8g2.nextPage());

  unsigned int w = vgafb.getWidth();
  unsigned int h = vgafb.getHeight();
  vgafb.setPixel(0, 0, 1);
  vgafb.setPixel(w - 1, 0, 1);
  vgafb.setPixel(w - 1, h - 1, 1);
  vgafb.setPixel(0, h - 1, 1);
  
  _delay_ms(6000);
  vgafb.scroll(300);
  _delay_ms(800);
}
