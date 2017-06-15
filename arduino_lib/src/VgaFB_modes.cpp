/*

VgaFB_modes.cpp

Minimalistic VGA framebuffer hardware and software for microcontrollers
https://github.com/tlaasik/vgafb/

---------------------------------------------------------------------------------------

Copyright (c) 2017, toomas.laasik@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or other
materials provided with the distribution.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------------------
*/
#include "VgaFB_core.h"

// VESA 640x480@75Hz (modified hTotal because 840/16=52.5. PxClk is not exact either, this mode uses 31.5MHz)
vgamode_t vgamode_640x480_75Hz_32MHz = { 640, 656, 720, 848,  480, 481, 484, 500,  1, (vgaflags_t)(VGAFLAG_INVERTED_VSYNC | VGAFLAG_INVERTED_HSYNC) };

//VESA 640x400@85Hz 640x400	31.5  640 672 736 832  400 401 404 445 -hsync +vsync
vgamode_t vgamode_640x400_85Hz_32MHz = { 640, 672, 736, 832,  400, 401, 404, 445,  1, VGAFLAG_INVERTED_HSYNC };

// 16MHz OK, aga sync on modifitseeritud. mult?
// VESA 800x600@60Hz hsync: 37.9kHz 40.0  800 840 968 1056  600 601 605 628  +hsync +vsync
vgamode_t vgamode_400x300_60Hz_20MHz = { 400, 420, 484, 526,  600, 601, 605, 628,  2, VGAFLAG_NONE };

// VESA 640x480@75Hz (modified hTotal because 840/16=52.5. PxClk is not exact either, this mode uses 31.5MHz)
vgamode_t vgamode_320x240_75Hz_16MHz = { 320, 328, 360, 424,  480, 481, 484, 500,  2, (vgaflags_t)(VGAFLAG_INVERTED_VSYNC | VGAFLAG_INVERTED_HSYNC) };

//VESA 640x400@85Hz 640x400	31.5  640 672 736 832  400 401 404 445 -hsync +vsync
vgamode_t vgamode_320x200_85Hz_16MHz = { 320, 336, 368, 416,  400, 401, 404, 445,  2, VGAFLAG_INVERTED_HSYNC };

vgamode_t vgamode_256x256_60Hz_20MHz = { 256, 352, 416, 526,  512, 601, 605, 628,  2, VGAFLAG_NONE };


// test
//vgamode_t vgamode_test = { 320, 336, 368, 416,  2, 4, 5, 6,  2, VGAFLAG_INVERTED_HSYNC };
