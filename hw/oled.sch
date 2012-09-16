EESchema Schematic File Version 2  date Sun 16 Sep 2012 03:37:22 PM EDT
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
LIBS:special
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
LIBS:cap
LIBS:ltc3559
LIBS:mt29fxx01a
LIBS:res
LIBS:usbmicroconn
LIBS:mma8452q
LIBS:l3gd20
LIBS:mag3110
LIBS:lps331ap
LIBS:stm32f405rg
LIBS:ind
LIBS:swd_10pin
LIBS:pushbutton
LIBS:xtal
LIBS:cc1101
LIBS:sb
LIBS:tp
LIBS:antenna
LIBS:ssd1306_ser
LIBS:lv_detect
LIBS:imu_logger-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 2 6
Title "IMU Logger"
Date "16 sep 2012"
Rev "a"
Comp "McGill University"
Comment1 "Ben Nahill (bnahill@gmail.com)"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 4800 3850
Wire Wire Line
	4800 4450 4800 3850
Wire Wire Line
	5300 4150 5650 4150
Wire Wire Line
	5300 4350 5650 4350
Wire Wire Line
	5300 4550 5650 4550
Wire Wire Line
	5300 4050 5650 4050
Wire Wire Line
	5650 4850 5550 4850
Wire Wire Line
	5550 4850 5550 4950
Wire Wire Line
	5650 4650 5150 4650
Wire Wire Line
	5150 4650 5150 4850
Connection ~ 5350 5300
Wire Wire Line
	5550 5150 5550 5300
Wire Wire Line
	5650 3750 5150 3750
Wire Wire Line
	5150 3750 5150 3650
Wire Wire Line
	5150 3650 5250 3650
Wire Wire Line
	5650 3550 5150 3550
Wire Wire Line
	5150 3550 5150 3450
Wire Wire Line
	5150 3450 5250 3450
Wire Wire Line
	5650 3450 5450 3450
Wire Wire Line
	5650 3650 5450 3650
Wire Wire Line
	5350 5150 5350 5400
Wire Wire Line
	5150 5300 5150 5150
Wire Wire Line
	5350 4950 5350 4750
Wire Wire Line
	5350 4750 5650 4750
Wire Wire Line
	4950 5300 4950 3950
Wire Wire Line
	4950 3950 5650 3950
Connection ~ 5150 5300
Wire Wire Line
	5650 3850 4700 3850
Wire Wire Line
	5650 4450 5300 4450
Wire Wire Line
	5650 4250 5300 4250
Wire Wire Line
	4800 4650 4800 5300
Wire Wire Line
	4800 5300 5550 5300
Connection ~ 4950 5300
$Comp
L CAP C37
U 1 1 504A0E3E
P 4800 4550
F 0 "C37" H 4820 4520 50  0000 L TNN
F 1 "1u" H 4825 4570 30  0000 L BNN
	1    4800 4550
	0    -1   -1   0   
$EndComp
Text HLabel 4700 3850 0    60   Input ~ 0
Vdd
Text HLabel 5300 4550 0    60   Input ~ 0
SDIN
Text HLabel 5300 4450 0    60   Input ~ 0
SCLK
Text HLabel 5300 4350 0    60   Input ~ 0
nD/C
Text HLabel 5300 4250 0    60   Input ~ 0
nRES
Text HLabel 5300 4150 0    60   Input ~ 0
nCS
Text HLabel 5300 4050 0    60   Input ~ 0
Vdd
$Comp
L SSD1306_SER J3
U 1 1 504A0D79
P 5850 4150
F 0 "J3" H 5950 3350 60  0000 C CNN
F 1 "SSD1306_SER" H 5900 4950 60  0000 C CNN
	1    5850 4150
	1    0    0    -1  
$EndComp
$Comp
L CAP C41
U 1 1 504A0CF7
P 5550 5050
F 0 "C41" H 5570 5020 50  0000 L TNN
F 1 "2.2u" H 5575 5070 30  0000 L BNN
	1    5550 5050
	0    -1   -1   0   
$EndComp
$Comp
L CAP C40
U 1 1 504A0CF4
P 5350 5050
F 0 "C40" H 5370 5020 50  0000 L TNN
F 1 "2.2u" H 5375 5070 30  0000 L BNN
	1    5350 5050
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR05
U 1 1 504A0CF1
P 5350 5400
F 0 "#PWR05" H 5350 5400 30  0001 C CNN
F 1 "GND" H 5350 5330 30  0001 C CNN
	1    5350 5400
	1    0    0    -1  
$EndComp
$Comp
L RES R3
U 1 1 504A0CDB
P 5150 5000
F 0 "R3" H 5220 4965 50  0000 L TNN
F 1 "390k" H 5150 5055 30  0000 C BNN
	1    5150 5000
	0    -1   -1   0   
$EndComp
$Comp
L CAP C39
U 1 1 504A0A07
P 5350 3650
F 0 "C39" H 5370 3620 50  0000 L TNN
F 1 "1u" H 5375 3670 30  0000 L BNN
	1    5350 3650
	-1   0    0    1   
$EndComp
$Comp
L CAP C38
U 1 1 504A09F8
P 5350 3450
F 0 "C38" H 5370 3420 50  0000 L TNN
F 1 "1u" H 5375 3470 30  0000 L BNN
	1    5350 3450
	-1   0    0    1   
$EndComp
$EndSCHEMATC
