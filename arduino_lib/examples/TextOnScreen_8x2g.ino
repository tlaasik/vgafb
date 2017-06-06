#include <U8x8lib.h>
#include <SPI.h>
#include <VgaFB_u8x8.h>

// note that third PWM on Nano is configured to output 8MHz clock which is then multiplied by
// 2.5 (set by jumpers) giving 20MHz. Since Arduino itself runs at 16MHz we tell that the multiplier is 1.25
U8G2_VGAFB_400X300_60Hz_20MHz_GENERIC_1_VGAFBBUS u8g2(U8G2_R0, /*mul/div=*/5, 4, /* cs=*/ 4, /* a=*/ 9);

void setup() {
  u8g2.begin();
}
void loop()
{
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, "VGA framebuffer for Arduino Nano");
    u8g2.drawStr(300, 350, "Running at 640x480 85Hz");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(300, 380, "Using u8g2 for rendering text");
    u8g2.drawStr(0, 40, "https://github.com/tlaasik/vgafb");
    u8g2.drawFilledEllipse(100, 100, 90, 30);
  } while (u8g2.nextPage());
}