EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:vga
LIBS:vga-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "VGA framebuffer"
Date ""
Rev "2"
Comp "Designed by Toomas Laasik"
Comment1 "https://github.com/tlaasik/vgafb"
Comment2 "Pixel clock multiplication part can be left out (U3 and related). Then PXCLK should be\\nused as pixel clock input. With U3 it's an output. D1 is optional too\\n "
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L VSS #PWR01
U 1 1 58ED3CB3
P 5850 7300
F 0 "#PWR01" H 5850 7150 50  0001 C CNN
F 1 "VSS" H 5850 7450 50  0000 C CNN
F 2 "" H 5850 7300 50  0000 C CNN
F 3 "" H 5850 7300 50  0000 C CNN
	1    5850 7300
	-1   0    0    1   
$EndComp
$Comp
L MB85RS16 U2
U 1 1 58ED42A4
P 5450 4000
F 0 "U2" H 5650 4250 50  0000 L CNN
F 1 "23LCV512" H 5000 4250 50  0000 L CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5100 3950 50  0001 C CNN
F 3 "" H 5100 3950 50  0000 C CNN
	1    5450 4000
	-1   0    0    -1  
$EndComp
$Comp
L CONN_01X15 P1
U 1 1 58ED4643
P 6600 2050
F 0 "P1" H 6600 2850 50  0000 C CNN
F 1 "CONN_01X15" V 6700 2050 50  0000 C CNN
F 2 "vga:con-subd-HDF15H" H 6600 2050 50  0001 C CNN
F 3 "" H 6600 2050 50  0000 C CNN
	1    6600 2050
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 58ED53D9
P 4750 1350
F 0 "R6" V 4830 1350 50  0000 C CNN
F 1 "470" V 4750 1350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4680 1350 50  0001 C CNN
F 3 "" H 4750 1350 50  0000 C CNN
	1    4750 1350
	0    1    -1   0   
$EndComp
$Comp
L R R5
U 1 1 58ED5AD5
P 4200 2350
F 0 "R5" V 4100 2350 50  0000 C CNN
F 1 "68" V 4200 2350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4130 2350 50  0001 C CNN
F 3 "" H 4200 2350 50  0000 C CNN
	1    4200 2350
	1    0    0    1   
$EndComp
$Comp
L R R8
U 1 1 58ED5C89
P 4400 2350
F 0 "R8" V 4300 2350 50  0000 C CNN
F 1 "68" V 4400 2350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4330 2350 50  0001 C CNN
F 3 "" H 4400 2350 50  0000 C CNN
	1    4400 2350
	1    0    0    1   
$EndComp
$Comp
L GND #PWR02
U 1 1 58ED5F03
P 6150 2050
F 0 "#PWR02" H 6150 1800 50  0001 C CNN
F 1 "GND" H 6150 1900 50  0000 C CNN
F 2 "" H 6150 2050 50  0000 C CNN
F 3 "" H 6150 2050 50  0000 C CNN
	1    6150 2050
	1    0    0    -1  
$EndComp
Text Label 6300 2450 2    60   ~ 0
SDA
Text Label 6300 2750 2    60   ~ 0
SCL
Text Label 6300 2550 2    60   ~ 0
HSYNC_OUT
Text Label 6300 2650 2    60   ~ 0
VSYNC_OUT
Text Label 6100 1350 0    60   ~ 0
PX_R
Text Label 6100 1450 0    60   ~ 0
PX_G
Text Label 6100 1550 0    60   ~ 0
PX_B
$Comp
L R R2
U 1 1 58ED8E56
P 2150 2250
F 0 "R2" V 2230 2250 50  0000 C CNN
F 1 "2k2" V 2150 2250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 2080 2250 50  0001 C CNN
F 3 "" H 2150 2250 50  0000 C CNN
	1    2150 2250
	-1   0    0    1   
$EndComp
$Comp
L R R3
U 1 1 58ED8EF9
P 2350 2250
F 0 "R3" V 2430 2250 50  0000 C CNN
F 1 "2k2" V 2350 2250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 2280 2250 50  0001 C CNN
F 3 "" H 2350 2250 50  0000 C CNN
	1    2350 2250
	-1   0    0    1   
$EndComp
$Comp
L VDD #PWR03
U 1 1 58ED917F
P 2350 2000
F 0 "#PWR03" H 2350 1850 50  0001 C CNN
F 1 "VDD" H 2350 2150 50  0000 C CNN
F 2 "" H 2350 2000 50  0000 C CNN
F 3 "" H 2350 2000 50  0000 C CNN
	1    2350 2000
	1    0    0    -1  
$EndComp
Text Label 2000 1700 2    60   ~ 0
VSYNC
Text Label 2000 1600 2    60   ~ 0
HSYNC
$Comp
L GND #PWR04
U 1 1 58EDD6AB
P 5450 4450
F 0 "#PWR04" H 5450 4200 50  0001 C CNN
F 1 "GND" H 5450 4300 50  0000 C CNN
F 2 "" H 5450 4450 50  0000 C CNN
F 3 "" H 5450 4450 50  0000 C CNN
	1    5450 4450
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR05
U 1 1 58EDD7F8
P 5450 3550
F 0 "#PWR05" H 5450 3400 50  0001 C CNN
F 1 "VDD" H 5450 3700 50  0000 C CNN
F 2 "" H 5450 3550 50  0000 C CNN
F 3 "" H 5450 3550 50  0000 C CNN
	1    5450 3550
	1    0    0    -1  
$EndComp
Text Label 2000 4550 2    60   ~ 0
MOSI
$Comp
L GND #PWR06
U 1 1 58EE0CB2
P 5650 7300
F 0 "#PWR06" H 5650 7050 50  0001 C CNN
F 1 "GND" H 5650 7150 50  0000 C CNN
F 2 "" H 5650 7300 50  0000 C CNN
F 3 "" H 5650 7300 50  0000 C CNN
	1    5650 7300
	1    0    0    -1  
$EndComp
Text Label 2000 4700 2    60   ~ 0
/SEL
$Comp
L R R4
U 1 1 58EE26DD
P 6250 3850
F 0 "R4" V 6330 3850 50  0000 C CNN
F 1 "2k2" V 6250 3850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 6180 3850 50  0001 C CNN
F 3 "" H 6250 3850 50  0000 C CNN
	1    6250 3850
	-1   0    0    1   
$EndComp
$Comp
L NB3N502 U3
U 1 1 58EE6CDE
P 3650 6450
F 0 "U3" H 3400 6700 60  0000 C CNN
F 1 "NB3N502" H 3750 6700 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 3650 6450 60  0001 C CNN
F 3 "" H 3650 6450 60  0001 C CNN
	1    3650 6450
	1    0    0    -1  
$EndComp
$Comp
L Crystal Y1
U 1 1 58EE75DB
P 3650 5500
F 0 "Y1" H 3650 5650 50  0000 C CNN
F 1 "10MHz" H 3650 5350 50  0000 C CNN
F 2 "Crystals:Crystal_SMD_5032_2Pads" H 3650 5500 50  0001 C CNN
F 3 "" H 3650 5500 50  0000 C CNN
	1    3650 5500
	1    0    0    -1  
$EndComp
$Comp
L C_Small C2
U 1 1 58EE7A0A
P 3350 5700
F 0 "C2" H 3360 5770 50  0000 L CNN
F 1 "1p" H 3360 5620 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 3350 5700 50  0001 C CNN
F 3 "" H 3350 5700 50  0000 C CNN
	1    3350 5700
	1    0    0    -1  
$EndComp
$Comp
L C_Small C3
U 1 1 58EE7F18
P 3950 5700
F 0 "C3" H 3960 5770 50  0000 L CNN
F 1 "1p" H 3960 5620 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 3950 5700 50  0001 C CNN
F 3 "" H 3950 5700 50  0000 C CNN
	1    3950 5700
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 58EE8308
P 3650 5850
F 0 "#PWR07" H 3650 5600 50  0001 C CNN
F 1 "GND" H 3650 5700 50  0000 C CNN
F 2 "" H 3650 5850 50  0000 C CNN
F 3 "" H 3650 5850 50  0000 C CNN
	1    3650 5850
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 58ED38D7
P 5650 7050
F 0 "C1" H 5675 7150 50  0000 L CNN
F 1 "10n" H 5675 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 5688 6900 50  0001 C CNN
F 3 "" H 5650 7050 50  0000 C CNN
	1    5650 7050
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR08
U 1 1 58EEA469
P 3150 6200
F 0 "#PWR08" H 3150 6050 50  0001 C CNN
F 1 "VDD" H 3150 6350 50  0000 C CNN
F 2 "" H 3150 6200 50  0000 C CNN
F 3 "" H 3150 6200 50  0000 C CNN
	1    3150 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 58EEAD89
P 3150 6700
F 0 "#PWR09" H 3150 6450 50  0001 C CNN
F 1 "GND" H 3150 6550 50  0000 C CNN
F 2 "" H 3150 6700 50  0000 C CNN
F 3 "" H 3150 6700 50  0000 C CNN
	1    3150 6700
	1    0    0    -1  
$EndComp
$Comp
L JUMPER3 JP2
U 1 1 58EED663
P 2950 5500
F 0 "JP2" H 3000 5400 50  0000 L CNN
F 1 "OSC_ONOFF" H 2950 5600 50  0000 C BNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 2950 5500 50  0001 C CNN
F 3 "" H 2950 5500 50  0000 C CNN
	1    2950 5500
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR010
U 1 1 58EF8D56
P 4900 5450
F 0 "#PWR010" H 4900 5300 50  0001 C CNN
F 1 "VDD" H 4900 5600 50  0000 C CNN
F 2 "" H 4900 5450 50  0000 C CNN
F 3 "" H 4900 5450 50  0000 C CNN
	1    4900 5450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR011
U 1 1 58EF93FF
P 4900 6150
F 0 "#PWR011" H 4900 5900 50  0001 C CNN
F 1 "GND" H 4900 6000 50  0000 C CNN
F 2 "" H 4900 6150 50  0000 C CNN
F 3 "" H 4900 6150 50  0000 C CNN
	1    4900 6150
	1    0    0    -1  
$EndComp
Text Label 2000 5500 2    60   ~ 0
CLKIN
Text Label 1300 5800 0    60   ~ 0
PXCLK
Text Label 3750 1350 0    60   ~ 0
PIXEL
$Comp
L C C4
U 1 1 58F095DA
P 6000 7050
F 0 "C4" H 6025 7150 50  0000 L CNN
F 1 "10n" H 6025 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 6038 6900 50  0001 C CNN
F 3 "" H 6000 7050 50  0000 C CNN
	1    6000 7050
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 58F0964C
P 6350 7050
F 0 "C5" H 6375 7150 50  0000 L CNN
F 1 "10n" H 6375 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 6388 6900 50  0001 C CNN
F 3 "" H 6350 7050 50  0000 C CNN
	1    6350 7050
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR012
U 1 1 58F09DF8
P 5650 6800
F 0 "#PWR012" H 5650 6650 50  0001 C CNN
F 1 "VDD" H 5650 6950 50  0000 C CNN
F 2 "" H 5650 6800 50  0000 C CNN
F 3 "" H 5650 6800 50  0000 C CNN
	1    5650 6800
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X03 P2
U 1 1 58F10412
P 5650 1450
F 0 "P2" H 5650 1650 50  0000 C CNN
F 1 "COLOR_SELECT" H 5650 1250 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" H 5650 250 50  0001 C CNN
F 3 "" H 5650 250 50  0000 C CNN
	1    5650 1450
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X14 P3
U 1 1 58F1C730
P 1100 6450
F 0 "P3" H 1100 7200 50  0000 C CNN
F 1 "CONN_01X14" V 1200 6450 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x14" H 1100 6450 50  0001 C CNN
F 3 "" H 1100 6450 50  0000 C CNN
	1    1100 6450
	-1   0    0    -1  
$EndComp
$Comp
L D_Small D1
U 1 1 58F240A6
P 5050 1600
F 0 "D1" H 5000 1680 50  0000 L CNN
F 1 "1N4148" H 4900 1520 50  0000 L CNN
F 2 "Diodes_SMD:SOD-323_HandSoldering" V 5050 1600 50  0001 C CNN
F 3 "" V 5050 1600 50  0000 C CNN
	1    5050 1600
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR013
U 1 1 58F292D7
P 5050 1700
F 0 "#PWR013" H 5050 1450 50  0001 C CNN
F 1 "GND" H 5050 1550 50  0000 C CNN
F 2 "" H 5050 1700 50  0000 C CNN
F 3 "" H 5050 1700 50  0000 C CNN
	1    5050 1700
	1    0    0    -1  
$EndComp
Text Label 1300 7100 0    60   ~ 0
SCL
Text Label 1300 7000 0    60   ~ 0
SDA
$Comp
L CONN_01X02 P4
U 1 1 58F2D65C
P 5050 7050
F 0 "P4" H 5050 7200 50  0000 C CNN
F 1 "CONN_01X02" V 5150 7050 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 5050 7050 50  0001 C CNN
F 3 "" H 5050 7050 50  0000 C CNN
	1    5050 7050
	-1   0    0    -1  
$EndComp
Text Label 1300 6800 0    60   ~ 0
GND
Text Label 1300 6700 0    60   ~ 0
SCK
Text Label 1300 6600 0    60   ~ 0
MISO
Text Label 1300 6500 0    60   ~ 0
MOSI
Text Label 1300 6400 0    60   ~ 0
VSYNC
Text Label 1300 6200 0    60   ~ 0
CLKIN
Text Label 1300 5900 0    60   ~ 0
HSYNC
Text Label 1300 6100 0    60   ~ 0
A
Text Label 1300 6000 0    60   ~ 0
B
Text Label 1300 6300 0    60   ~ 0
/SEL
$Comp
L CONN_02X03 P5
U 1 1 58F4275F
P 4900 5800
F 0 "P5" H 4900 6000 50  0000 C CNN
F 1 "CLOCK_SELECT" H 4900 5600 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" H 4900 4600 50  0001 C CNN
F 3 "" H 4900 4600 50  0000 C CNN
	1    4900 5800
	1    0    0    -1  
$EndComp
Text Label 2000 3800 2    60   ~ 0
PXCLK
$Comp
L C C6
U 1 1 59083965
P 6500 4250
F 0 "C6" H 6525 4350 50  0000 L CNN
F 1 "100p" H 6525 4150 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 6538 4100 50  0001 C CNN
F 3 "" H 6500 4250 50  0000 C CNN
	1    6500 4250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 59083DE4
P 6500 4400
F 0 "#PWR014" H 6500 4150 50  0001 C CNN
F 1 "GND" H 6500 4250 50  0000 C CNN
F 2 "" H 6500 4400 50  0000 C CNN
F 3 "" H 6500 4400 50  0000 C CNN
	1    6500 4400
	1    0    0    -1  
$EndComp
$Comp
L 74HC4053 U1
U 1 1 59369171
P 4000 3900
F 0 "U1" H 4000 4150 50  0000 C CNN
F 1 "74HC4053" H 4200 3700 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-16_3.9x9.9mm_Pitch1.27mm" H 4000 3900 50  0001 C CNN
F 3 "" H 4000 3900 50  0000 C CNN
	1    4000 3900
	1    0    0    -1  
$EndComp
$Comp
L 74HC4053 U1
U 2 1 59369DAA
P 3100 2650
F 0 "U1" H 3100 2900 50  0000 C CNN
F 1 "74HC4053" H 3300 2450 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-16_3.9x9.9mm_Pitch1.27mm" H 3100 2650 50  0001 C CNN
F 3 "" H 3100 2650 50  0000 C CNN
	2    3100 2650
	1    0    0    -1  
$EndComp
$Comp
L 74HC4053 U1
U 3 1 5936BA74
P 4000 3100
F 0 "U1" H 4000 3350 50  0000 C CNN
F 1 "74HC4053" H 4200 2900 50  0000 C CNN
F 2 "Housings_SOIC:SOIC-16_3.9x9.9mm_Pitch1.27mm" H 4000 3100 50  0001 C CNN
F 3 "" H 4000 3100 50  0000 C CNN
	3    4000 3100
	1    0    0    -1  
$EndComp
Text Label 2000 2550 2    60   ~ 0
A
Text Label 2000 2750 2    60   ~ 0
B
Text Label 2000 4000 2    60   ~ 0
SCK
Text Label 2000 3200 2    60   ~ 0
MISO
Wire Wire Line
	6400 1750 6300 1750
Wire Wire Line
	6300 1750 6300 2250
Wire Wire Line
	6300 1850 6400 1850
Wire Wire Line
	6150 1950 6400 1950
Connection ~ 6300 1850
Wire Wire Line
	6300 2050 6400 2050
Connection ~ 6300 1950
Connection ~ 6300 2050
Wire Wire Line
	6300 2250 6400 2250
Wire Wire Line
	6150 1950 6150 2050
Wire Wire Line
	5900 1350 6400 1350
Wire Wire Line
	6300 2450 6400 2450
Wire Wire Line
	6300 2750 6400 2750
Wire Wire Line
	5850 3900 5950 3900
Wire Wire Line
	5950 3900 5950 3650
Wire Wire Line
	5450 3650 6250 3650
Wire Wire Line
	5450 4300 5450 4450
Wire Wire Line
	5850 4000 5950 4000
Wire Wire Line
	5950 4000 5950 4350
Wire Wire Line
	5950 4350 5450 4350
Connection ~ 5450 4350
Wire Wire Line
	5850 4100 6500 4100
Connection ~ 6250 4100
Wire Wire Line
	6250 3650 6250 3700
Connection ~ 5950 3650
Wire Wire Line
	2150 2100 2150 2050
Wire Wire Line
	2150 2050 2350 2050
Wire Wire Line
	2350 2000 2350 2100
Connection ~ 2350 2050
Wire Wire Line
	5450 3550 5450 3700
Connection ~ 5450 3650
Wire Wire Line
	3800 5500 4100 5500
Wire Wire Line
	3200 5500 3500 5500
Wire Wire Line
	4100 5500 4100 6300
Wire Wire Line
	3950 5600 3950 5500
Connection ~ 3950 5500
Wire Wire Line
	3350 5500 3350 5600
Connection ~ 3350 5500
Wire Wire Line
	3950 5850 3950 5800
Wire Wire Line
	3350 5850 3950 5850
Wire Wire Line
	3350 5850 3350 5800
Connection ~ 3650 5850
Wire Wire Line
	4100 6300 4050 6300
Wire Wire Line
	2950 6300 3250 6300
Wire Wire Line
	2950 5600 2950 6300
Wire Wire Line
	2000 5500 2700 5500
Wire Wire Line
	3150 6400 3250 6400
Wire Wire Line
	3150 6200 3150 6400
Wire Wire Line
	3150 6500 3150 6700
Wire Wire Line
	3150 6500 3250 6500
Wire Wire Line
	2300 3800 2300 7100
Wire Wire Line
	4950 4000 5050 4000
Wire Wire Line
	6250 4000 6250 4700
Wire Wire Line
	4050 6600 4100 6600
Wire Wire Line
	4100 6600 4100 7100
Wire Wire Line
	5650 6800 5650 6900
Wire Wire Line
	5250 6850 6350 6850
Wire Wire Line
	6000 6850 6000 6900
Connection ~ 5650 6850
Wire Wire Line
	6350 6850 6350 6900
Connection ~ 6000 6850
Wire Wire Line
	6350 7250 6350 7200
Wire Wire Line
	5250 7250 6350 7250
Wire Wire Line
	5650 7200 5650 7300
Connection ~ 5650 7250
Wire Wire Line
	6000 7200 6000 7250
Connection ~ 6000 7250
Wire Wire Line
	5900 1450 6400 1450
Wire Wire Line
	6400 1550 5900 1550
Wire Wire Line
	5350 1550 5400 1550
Wire Wire Line
	5350 1350 5350 1550
Wire Wire Line
	5350 1450 5400 1450
Wire Wire Line
	4900 1350 5400 1350
Connection ~ 5350 1450
Connection ~ 5350 1350
Wire Wire Line
	5050 1500 5050 1350
Connection ~ 5050 1350
Wire Wire Line
	5250 7000 5250 6850
Wire Wire Line
	5250 7250 5250 7100
Wire Wire Line
	5850 7300 5850 7250
Connection ~ 5850 7250
Wire Wire Line
	4350 6400 4050 6400
Wire Wire Line
	4050 6500 5400 6500
Wire Wire Line
	4650 5900 4600 5900
Wire Wire Line
	4600 5900 4600 6100
Wire Wire Line
	4600 6100 5200 6100
Wire Wire Line
	5200 6100 5200 5900
Wire Wire Line
	5200 5900 5150 5900
Wire Wire Line
	4650 5700 4600 5700
Wire Wire Line
	4600 5700 4600 5500
Wire Wire Line
	4600 5500 5200 5500
Wire Wire Line
	5200 5500 5200 5700
Wire Wire Line
	5200 5700 5150 5700
Wire Wire Line
	4900 5450 4900 5500
Connection ~ 4900 5500
Wire Wire Line
	4900 6150 4900 6100
Connection ~ 4900 6100
Wire Wire Line
	4650 5800 4350 5800
Wire Wire Line
	4350 5800 4350 6400
Wire Wire Line
	5400 6500 5400 5800
Wire Wire Line
	5400 5800 5150 5800
Wire Wire Line
	4100 7100 2300 7100
Wire Wire Line
	2000 2550 2800 2550
Wire Wire Line
	2000 2750 2800 2750
Wire Wire Line
	2350 2550 2350 2400
Connection ~ 2350 2550
Wire Wire Line
	2150 2750 2150 2400
Connection ~ 2150 2750
Wire Wire Line
	4450 4100 5050 4100
Wire Wire Line
	4450 4100 4450 3100
Wire Wire Line
	4400 2550 6400 2550
Wire Wire Line
	6250 4700 2000 4700
Wire Wire Line
	2000 4000 3700 4000
Wire Wire Line
	2000 3800 3700 3800
Wire Wire Line
	4000 3350 4000 3450
Wire Wire Line
	4000 4150 4000 4250
Wire Wire Line
	4450 3100 4300 3100
Wire Wire Line
	4300 3900 5050 3900
Wire Wire Line
	2000 1700 4200 1700
Wire Wire Line
	4200 2650 6400 2650
Wire Wire Line
	3450 2650 3450 4250
Wire Wire Line
	4000 3450 3450 3450
Wire Wire Line
	3450 4250 4000 4250
Connection ~ 3450 3450
Wire Wire Line
	3100 2900 3100 2950
Wire Wire Line
	3100 2950 2650 2950
Connection ~ 2650 1700
Wire Wire Line
	3650 3000 3700 3000
Wire Wire Line
	3650 1350 3650 3000
Wire Wire Line
	4200 2650 4200 2500
Wire Wire Line
	4400 2550 4400 2500
Wire Wire Line
	4400 1600 2000 1600
Wire Wire Line
	4200 1700 4200 2200
Wire Wire Line
	4400 2200 4400 1600
Wire Wire Line
	4600 1350 3650 1350
Wire Wire Line
	2650 2950 2650 1700
Wire Wire Line
	3400 2650 3450 2650
Wire Wire Line
	2000 3200 3700 3200
Wire Wire Line
	2000 4550 4950 4550
Wire Wire Line
	4950 4550 4950 4000
Connection ~ 2300 3800
Text Label 1300 6900 0    60   ~ 0
VDD
$EndSCHEMATC
