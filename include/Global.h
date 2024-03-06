/*
===============================================================================
Name        : Global.cpp
Author      : Junxuan Kao
Version     : 0.0.1
Date         :
Copyright   :
License  :
Description : Global definition
===============================================================================
                                        History
---------+---------+--------------------------------------------+-------------
DATE     |  VERSION |   DESCRIPTIONS                             |  By
---------+---------+--------------------------------------------+-------------
19/08/07   0.0.1     Initial version                             Junxuan Kao
===============================================================================
*/

#include "mbed.h"
#include "uart_Service.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "RingBuffer.h"

#ifndef GLOBAL_H_
#define GLOBAL_H_

/*
TODO: insert global definitions and declarations here
*/
#define BaudRate 115200
#define Char2Sign16(a) (a & 0x80)?(0xFF00 | (0x00FF & a)): (0x00FF & a)

extern uartService* pc;

extern DigitalOut LD1;
extern DigitalOut LD2;
extern DigitalOut LD3;

extern bool hexChk(string &str);
extern float Stofloat(string str);
extern int StoNum(string str);

extern void EVB_INITIAL_SSD1306(void);
extern void EVB_INITIAL_CANSERVICE(void);
extern void EVB_INITIAL_UARTSERVICE(void);
extern void EVB_INITIAL_LOWSPEEDDOUTBUS(void);
#endif /*GLOBAL_H_*/
