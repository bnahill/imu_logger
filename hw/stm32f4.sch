EESchema Schematic File Version 2  date Tue 21 Aug 2012 11:12:24 AM EDT
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
LIBS:imu_logger-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 2 4
Title ""
Date "21 aug 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 4600 2800
Wire Wire Line
	4600 2800 4850 2800
Wire Wire Line
	4850 2800 4850 2750
Wire Wire Line
	5550 2150 5550 2100
Wire Wire Line
	5550 2100 5450 2100
Wire Wire Line
	5450 2100 5450 2850
Connection ~ 3950 4700
Wire Wire Line
	3550 4700 4150 4700
Wire Wire Line
	4150 4700 4150 4600
Connection ~ 3550 4700
Wire Wire Line
	3950 4600 3950 4700
Connection ~ 3950 4350
Wire Wire Line
	4750 4350 3850 4350
Connection ~ 5250 5100
Wire Wire Line
	5250 4950 5250 5150
Connection ~ 5150 5400
Wire Wire Line
	5250 5350 5250 5400
Wire Wire Line
	5250 5400 5150 5400
Connection ~ 6800 5000
Wire Wire Line
	6800 5050 6800 5000
Wire Wire Line
	6600 5350 6600 5300
Wire Wire Line
	6450 5250 6450 5300
Wire Wire Line
	6550 4950 6550 5000
Wire Wire Line
	6550 5000 6950 5000
Connection ~ 7600 3550
Wire Wire Line
	7700 3400 7700 3550
Wire Wire Line
	7700 3550 7550 3550
Wire Wire Line
	7550 3300 7550 3250
Wire Wire Line
	7550 3250 6850 3250
Wire Wire Line
	5150 2850 5150 2100
Wire Wire Line
	5150 2100 5250 2100
Wire Wire Line
	5250 2100 5250 2150
Wire Wire Line
	4600 2850 4600 2750
Wire Wire Line
	4250 2450 5050 2450
Wire Wire Line
	5050 2450 5050 2850
Wire Wire Line
	4400 2450 4400 3150
Wire Wire Line
	4400 3150 4750 3150
Connection ~ 4400 2450
Wire Wire Line
	4600 2550 4600 2450
Connection ~ 4600 2450
Wire Wire Line
	6850 3150 7850 3150
Wire Wire Line
	7550 3550 7550 3500
Wire Wire Line
	7600 3600 7600 3550
Wire Wire Line
	7700 3200 7700 3150
Connection ~ 7700 3150
Wire Wire Line
	6450 4950 6450 5050
Wire Wire Line
	6450 5300 6800 5300
Wire Wire Line
	6800 5300 6800 5250
Connection ~ 6600 5300
Wire Wire Line
	5150 5450 5150 4950
Wire Wire Line
	5300 5100 5250 5100
Wire Wire Line
	3950 4400 3950 4350
Wire Wire Line
	3550 4750 3550 4250
Wire Wire Line
	3550 4250 4750 4250
Wire Wire Line
	4150 4350 4150 4400
Connection ~ 4150 4350
Wire Wire Line
	3400 3750 4750 3750
Wire Wire Line
	4850 2550 4850 2450
Connection ~ 4850 2450
$Comp
L CAP C?
U 1 1 5033A53B
P 4850 2650
F 0 "C?" H 4870 2620 50  0000 L TNN
F 1 "0.1u" H 4875 2670 30  0000 L BNN
	1    4850 2650
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5033A51F
P 5550 2150
F 0 "#PWR?" H 5550 2150 30  0001 C CNN
F 1 "GND" H 5550 2080 30  0001 C CNN
	1    5550 2150
	1    0    0    -1  
$EndComp
Text HLabel 3400 3750 0    60   Input ~ 0
nRESET
$Comp
L CAP C?
U 1 1 5033A494
P 4150 4500
F 0 "C?" H 4170 4470 50  0000 L TNN
F 1 "100p" H 4175 4520 30  0000 L BNN
	1    4150 4500
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 00000000
P 3550 4750
F 0 "#PWR?" H 3550 4750 30  0001 C CNN
F 1 "GND" H 3550 4680 30  0001 C CNN
	1    3550 4750
	1    0    0    -1  
$EndComp
$Comp
L CAP C?
U 1 1 5033A480
P 3950 4500
F 0 "C?" H 3970 4470 50  0000 L TNN
F 1 "0.1u" H 3975 4520 30  0000 L BNN
	1    3950 4500
	0    -1   -1   0   
$EndComp
Text HLabel 3850 4350 0    60   Input ~ 0
Vdd
Text HLabel 5300 5100 2    60   Input ~ 0
Vdd
$Comp
L CAP C?
U 1 1 5033A457
P 5250 5250
F 0 "C?" H 5270 5220 50  0000 L TNN
F 1 "0.1u" H 5275 5270 30  0000 L BNN
	1    5250 5250
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5033A450
P 5150 5450
F 0 "#PWR?" H 5150 5450 30  0001 C CNN
F 1 "GND" H 5150 5380 30  0001 C CNN
	1    5150 5450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 00000000
P 6600 5350
F 0 "#PWR?" H 6600 5350 30  0001 C CNN
F 1 "GND" H 6600 5280 30  0001 C CNN
	1    6600 5350
	1    0    0    -1  
$EndComp
Text HLabel 6950 5000 2    60   Input ~ 0
Vdd
$Comp
L CAP C?
U 1 1 5033A42A
P 6800 5150
F 0 "C?" H 6820 5120 50  0000 L TNN
F 1 "0.1u" H 6825 5170 30  0000 L BNN
	1    6800 5150
	0    -1   -1   0   
$EndComp
$Comp
L CAP C?
U 1 1 5033A422
P 6450 5150
F 0 "C?" H 6470 5120 50  0000 L TNN
F 1 "2.2u" H 6475 5170 30  0000 L BNN
	1    6450 5150
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5033A38E
P 7600 3600
F 0 "#PWR?" H 7600 3600 30  0001 C CNN
F 1 "GND" H 7600 3530 30  0001 C CNN
	1    7600 3600
	1    0    0    -1  
$EndComp
$Comp
L CAP C?
U 1 1 5033A382
P 7700 3300
F 0 "C?" H 7720 3270 50  0000 L TNN
F 1 "0.1u" H 7725 3320 30  0000 L BNN
	1    7700 3300
	0    -1   -1   0   
$EndComp
$Comp
L CAP C?
U 1 1 5033A378
P 7550 3400
F 0 "C?" H 7570 3370 50  0000 L TNN
F 1 "2.2u" H 7575 3420 30  0000 L BNN
	1    7550 3400
	0    -1   -1   0   
$EndComp
Text HLabel 7850 3150 2    60   Input ~ 0
Vdd
$Comp
L GND #PWR?
U 1 1 5033A35B
P 4600 2850
F 0 "#PWR?" H 4600 2850 30  0001 C CNN
F 1 "GND" H 4600 2780 30  0001 C CNN
	1    4600 2850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 00000000
P 5250 2150
F 0 "#PWR?" H 5250 2150 30  0001 C CNN
F 1 "GND" H 5250 2080 30  0001 C CNN
	1    5250 2150
	1    0    0    -1  
$EndComp
$Comp
L CAP C?
U 1 1 00000000
P 4600 2650
F 0 "C?" H 4620 2620 50  0000 L TNN
F 1 "1u" H 4625 2670 30  0000 L BNN
	1    4600 2650
	0    -1   -1   0   
$EndComp
Text HLabel 4250 2450 0    60   Input ~ 0
Vdd
$Comp
L STM32F405RG U?
U 1 1 00000000
P 5800 3900
F 0 "U?" H 6650 2900 60  0000 C CNN
F 1 "STM32F405RG" H 6900 4900 60  0000 C CNN
	1    5800 3900
	1    0    0    -1  
$EndComp
$EndSCHEMATC
