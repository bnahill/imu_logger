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
Wire Wire Line
	6450 3200 6050 3200
Wire Wire Line
	5650 2300 5500 2300
Wire Wire Line
	5500 2300 5500 2750
Wire Wire Line
	5050 3200 4950 3200
Wire Wire Line
	4950 3200 4950 3250
Wire Wire Line
	5400 2750 5400 2650
Wire Wire Line
	5400 2650 5250 2650
Wire Wire Line
	5750 2500 5600 2500
Wire Wire Line
	5600 2500 5600 2750
Connection ~ 6250 3550
Wire Wire Line
	6250 3700 6250 3400
Wire Wire Line
	6250 3400 6050 3400
Connection ~ 5800 4550
Wire Wire Line
	5850 4550 5600 4550
Wire Wire Line
	5850 4550 5850 4300
Wire Wire Line
	5800 4550 5800 4600
Wire Wire Line
	5600 4550 5600 4500
Wire Wire Line
	5400 3850 5400 3750
Wire Wire Line
	5600 3950 5600 3750
Connection ~ 4500 4550
Wire Wire Line
	4950 4450 4950 4550
Wire Wire Line
	4950 4550 4150 4550
Wire Wire Line
	4950 4250 4950 4200
Wire Wire Line
	4950 4200 4700 4200
Connection ~ 4700 3400
Wire Wire Line
	4700 3800 4700 3400
Connection ~ 4700 4100
Wire Wire Line
	4500 4100 4750 4100
Wire Wire Line
	4500 4050 4500 4150
Wire Wire Line
	4500 4550 4500 4450
Wire Wire Line
	4300 4550 4300 4600
Wire Wire Line
	4150 3700 4150 3300
Wire Wire Line
	4150 3300 5050 3300
Wire Wire Line
	3350 2900 3150 2900
Wire Wire Line
	3350 3100 3150 3100
Wire Wire Line
	4500 3750 4500 3400
Wire Wire Line
	4500 3400 5050 3400
Wire Wire Line
	3350 3250 3150 3250
Wire Wire Line
	4150 4550 4150 4000
Connection ~ 4300 4550
Connection ~ 4500 4100
Wire Wire Line
	4700 4250 4700 4000
Connection ~ 4700 4200
Wire Wire Line
	4700 4450 4700 4550
Connection ~ 4700 4550
Wire Wire Line
	5350 4100 5500 4100
Wire Wire Line
	5500 4100 5500 3750
Wire Wire Line
	5600 4150 5600 4200
Wire Wire Line
	5850 4100 5850 3900
Wire Wire Line
	6250 3550 6150 3550
Wire Wire Line
	5700 2750 5700 2650
Wire Wire Line
	5700 2650 6300 2650
Wire Wire Line
	6050 2500 6300 2500
Wire Wire Line
	6300 2500 6300 2750
Connection ~ 6300 2650
Wire Wire Line
	4700 3150 4700 3100
Wire Wire Line
	4700 3100 5050 3100
Wire Wire Line
	5850 3900 5600 3900
Connection ~ 5600 3900
Wire Wire Line
	6450 3100 6050 3100
Text HLabel 6450 3200 2    60   Input ~ 0
SUSP_EN
Text HLabel 5650 2300 2    60   Output ~ 0
nCHRG
Text HLabel 6450 3100 2    60   Input ~ 0
HPWR_EN
NoConn ~ 5700 3750
NoConn ~ 6050 3300
Text Label 4950 3250 2    60   ~ 0
Vbat
$Comp
L GND #PWR?
U 1 1 00000000
P 4700 3150
F 0 "#PWR?" H 4700 3150 30  0001 C CNN
F 1 "GND" H 4700 3080 30  0001 C CNN
	1    4700 3150
	1    0    0    -1  
$EndComp
Text Label 5250 2650 2    60   ~ 0
Vbus
$Comp
L GND #PWR?
U 1 1 00000000
P 6300 2750
F 0 "#PWR?" H 6300 2750 30  0001 C CNN
F 1 "GND" H 6300 2680 30  0001 C CNN
	1    6300 2750
	1    0    0    -1  
$EndComp
$Comp
L RES R?
U 1 1 00000000
P 5900 2500
F 0 "R?" H 5970 2465 50  0000 L TNN
F 1 "1.74k(1%)" H 5900 2555 30  0000 C BNN
	1    5900 2500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 00000000
P 6250 3700
F 0 "#PWR?" H 6250 3700 30  0001 C CNN
F 1 "GND" H 6250 3630 30  0001 C CNN
	1    6250 3700
	1    0    0    -1  
$EndComp
$Comp
L CAP C?
U 1 1 5032BB7C
P 5850 4200
F 0 "C?" H 5870 4170 50  0000 L TNN
F 1 "2.2u" H 5875 4220 30  0000 L BNN
	1    5850 4200
	0    -1   -1   0   
$EndComp
Text Label 5850 3900 0    60   ~ 0
Vbat
Text Label 5400 3850 2    60   ~ 0
Vbat
$Comp
L CAP C?
U 1 1 5032BB2C
P 5600 4050
F 0 "C?" H 5620 4020 50  0000 L TNN
F 1 "10u" H 5625 4070 30  0000 L BNN
	1    5600 4050
	0    -1   -1   0   
$EndComp
$Comp
L CAP C?
U 1 1 5032BAF4
P 4950 4350
F 0 "C?" H 4970 4320 50  0000 L TNN
F 1 "470n" H 4975 4370 30  0000 L BNN
	1    4950 4350
	0    -1   -1   0   
$EndComp
$Comp
L CAP C?
U 1 1 5032BAEF
P 4700 4350
F 0 "C?" H 4720 4320 50  0000 L TNN
F 1 "10u" H 4725 4370 30  0000 L BNN
	1    4700 4350
	0    -1   -1   0   
$EndComp
Text Label 4750 4200 0    60   ~ 0
Vreg
$Comp
L INDUCTOR L?
U 1 1 00000000
P 5050 4100
F 0 "L?" V 5000 4100 40  0000 C CNN
F 1 "4.7u" V 5150 4100 40  0000 C CNN
	1    5050 4100
	0    -1   -1   0   
$EndComp
$Comp
L CAP C?
U 1 1 00000000
P 4700 3900
F 0 "C?" H 4720 3870 50  0000 L TNN
F 1 "22p" H 4725 3920 30  0000 L BNN
	1    4700 3900
	0    -1   -1   0   
$EndComp
Text Label 3350 3250 0    60   ~ 0
Vbat
Text HLabel 3150 3250 0    60   Output ~ 0
Vbat
$Comp
L GND #PWR?
U 1 1 5032BA5E
P 5800 4600
F 0 "#PWR?" H 5800 4600 30  0001 C CNN
F 1 "GND" H 5800 4530 30  0001 C CNN
	1    5800 4600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 00000000
P 4300 4600
F 0 "#PWR?" H 4300 4600 30  0001 C CNN
F 1 "GND" H 4300 4530 30  0001 C CNN
	1    4300 4600
	1    0    0    -1  
$EndComp
Text Label 3350 3100 0    60   ~ 0
Vreg
Text Label 3350 2900 0    60   ~ 0
Vbus
$Comp
L RES R?
U 1 1 5032BA1A
P 5600 4350
F 0 "R?" H 5670 4315 50  0000 L TNN
F 1 "1R" H 5600 4405 30  0000 C BNN
	1    5600 4350
	0    -1   -1   0   
$EndComp
$Comp
L RES R?
U 1 1 5032BA03
P 4150 3850
AR Path="/5032B8A6/00000000" Ref="R?"  Part="1" 
AR Path="/5032B8A6/5032BA03" Ref="R?"  Part="1" 
F 0 "R?" H 4220 3815 50  0000 L TNN
F 1 "220k" H 4150 3905 30  0000 C BNN
	1    4150 3850
	0    -1   -1   0   
$EndComp
$Comp
L RES R?
U 1 1 5032B9FE
P 4500 4300
AR Path="/5032B8A6/00000000" Ref="R?"  Part="1" 
AR Path="/5032B8A6/5032B9FE" Ref="R?"  Part="1" 
F 0 "R?" H 4570 4265 50  0000 L TNN
F 1 "680k(1%)" H 4500 4355 30  0000 C BNN
	1    4500 4300
	0    -1   -1   0   
$EndComp
$Comp
L RES R?
U 1 1 00000000
P 4500 3900
F 0 "R?" H 4570 3865 50  0000 L TNN
F 1 "220k(1%)" H 4500 3955 30  0000 C BNN
	1    4500 3900
	0    -1   -1   0   
$EndComp
Text HLabel 3150 3100 0    60   Output ~ 0
Vreg
Text HLabel 3150 2900 0    60   Input ~ 0
Vbus
$Comp
L LTC3559 U?
U 1 1 00000000
P 5550 3250
F 0 "U?" H 5800 2800 60  0000 L TNN
F 1 "LTC3559" H 5750 3700 60  0000 L BNN
	1    5550 3250
	1    0    0    -1  
$EndComp
$EndSCHEMATC