# VgaFB

!!! This page is WIP !!!

Minimalistic VGA framebuffer hardware and software for microcontrollers (right now only tested on Arduino Uno and Nano).

Features:
* consists of only one 3x2:1 mux and 64KB SPI SRAM chip, clock part is optional
* easy to reconfigure for most SPI memory chips (EEPROM, Flash, FRAM)
* supports 2 colors: black and a color selected by RGB jumper
* up to 640x480 resolution (64KB address space, 1px per pixel)
* multiplies MCU clock by 2x 2.5x 3x 4x 5x to get pixel clock
* generates pixel clocks 20MHz 25MHz 30Mhz 40MHz 50MHz that should be used for MCU clock
* fast screen clear and fast vertical scrolling

Host MCU requirements:
* one 8bit PWM for hsync
* one 16bit PWM for vsync
* hardware SPI interface

Low level C API (not safe, buf fast):
```C
void VgaFB_ConfigBoard(vgafb_t * vgafb, uint8_t mul, uint8_t div, uint8_t cs_pin, uint8_t a_pin, uint8_t b_pin);
bool VgaFB_Begin(vgafb_t * vgafb, uint16_t hVisible, uint16_t hSyncStart, uint16_t hSyncEnd, uint16_t hTotal,
	uint16_t vVisible, uint16_t vSyncStart, uint16_t vSyncEnd, uint16_t vTotal, uint8_t flags);
void VgaFB_End(vgafb_t * vgafb);
void VgaFB_Clear(vgafb_t * vgafb);
void VgaFB_ClearScanline(vgafb_t * vgafb, int16_t scanline);
void VgaFB_Scroll(vgafb_t * vgafb, int16_t deltaScanline);
void VgaFB_Write(vgafb_t* vgafb, uint16_t dst, uint8_t* src, uint8_t cnt);
void VgaFB_Read(vgafb_t* vgafb, uint16_t src, uint8_t* dst, uint8_t cnt);
```

Has integraion with high level u8g2 and u8x8 API. These provide monochrome font and graphics rendering.
