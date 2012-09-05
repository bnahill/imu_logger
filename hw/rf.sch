EESchema Schematic File Version 2  date Tue 04 Sep 2012 12:42:10 PM EDT
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
LIBS:imu_logger-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 2 5
Title ""
Date "4 sep 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 5900 2400
Wire Wire Line
	5900 2300 5900 2800
Wire Wire Line
	5400 2000 5400 2900
Connection ~ 5750 2800
Wire Wire Line
	5750 2750 5750 2800
Wire Wire Line
	6050 2500 6050 1700
Wire Wire Line
	6050 1700 5300 1700
Wire Wire Line
	5300 1700 5300 2900
Wire Wire Line
	4500 3500 4700 3500
Wire Wire Line
	6100 3500 6200 3500
Wire Wire Line
	6200 3500 6200 3400
Wire Wire Line
	6200 3400 6100 3400
Wire Wire Line
	3750 3700 4700 3700
Wire Wire Line
	4500 3400 4700 3400
Wire Wire Line
	5200 2700 5200 2900
Wire Wire Line
	5600 2900 5600 2800
Wire Wire Line
	5600 2800 5900 2800
Wire Wire Line
	5900 2400 6050 2400
Connection ~ 6050 2400
Wire Wire Line
	5750 2450 5750 2300
Wire Wire Line
	5750 2300 5500 2300
Wire Wire Line
	5500 2300 5500 2900
Wire Wire Line
	5900 2100 5900 2050
Wire Wire Line
	5900 2050 5400 2050
Connection ~ 5400 2050
$Comp
L CAP C?
U 1 1 50462F5B
P 5900 2200
F 0 "C?" H 5920 2170 50  0000 L TNN
F 1 "CAP" H 5925 2220 30  0000 L BNN
	1    5900 2200
	0    -1   -1   0   
$EndComp
Text HLabel 5400 2000 1    60   Input ~ 0
Vdd
$Comp
L RES R?
U 1 1 50462F15
P 5750 2600
F 0 "R?" H 5820 2565 50  0000 L TNN
F 1 "RES" H 5750 2655 30  0000 C BNN
	1    5750 2600
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 50462EFE
P 6050 2500
F 0 "#PWR?" H 6050 2500 30  0001 C CNN
F 1 "GND" H 6050 2430 30  0001 C CNN
	1    6050 2500
	1    0    0    -1  
$EndComp
Text HLabel 6800 3400 2    60   Input ~ 0
Vdd
Text HLabel 3750 3700 0    60   Input ~ 0
Vdd
Text HLabel 4500 3400 0    60   Input ~ 0
SCLK
Text HLabel 4500 3500 0    60   Output ~ 0
SDO
Text HLabel 5200 2700 1    60   Input ~ 0
SDI
$Comp
L CC1101 U?
U 1 1 50462E7F
P 5400 3600
F 0 "U?" H 5750 2900 60  0000 C CNN
F 1 "CC1101" H 5800 4250 60  0000 C CNN
	1    5400 3600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
