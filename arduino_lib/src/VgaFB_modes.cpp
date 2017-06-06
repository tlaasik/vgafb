
#include "VgaFB_core.h"

// VESA 640x480@75Hz (modified hTotal because 840/16=52.5. PxClk is not exact either, this mode uses 31.5MHz)
vgamode_t vgamode_640x480_75Hz_32MHz = { 640, 656, 720, 848,  480, 481, 484, 500,  1, VGA_INVERTED_VSYNC | VGA_INVERTED_HSYNC };

// 16MHz OK, aga sync on modifitseeritud. mult?
// VESA 800x600@60Hz hsync: 37.9kHz 40.0  800 840 968 1056  600 601 605 628  +hsync +vsync
vgamode_t vgamode_400x300_60Hz_20MHz = { 400, 420, 484, 526,  600, 601, 605, 628,  2, 0 };

// VESA 640x480@75Hz (modified hTotal because 840/16=52.5. PxClk is not exact either, this mode uses 31.5MHz)
vgamode_t vgamode_320x240_75Hz_16MHz = { 320, 328, 360, 424,  480, 481, 484, 500,  2, VGA_INVERTED_VSYNC | VGA_INVERTED_HSYNC };

//VESA 640x400@85Hz 640x400	31.5  640 672 736 832  400 401 404 445 -hsync +vsync
vgamode_t vgamode_320x200_85Hz_16MHz = { 320, 336, 368, 416,  400, 401, 404, 445,  2, VGA_INVERTED_HSYNC };


// test
//VgaInit(16000000, 320, 336, 368, 416,  2, 4, 5, 6, VGA_INVERTED_HSYNC | VGA_DOUBLESCAN);
