/*
===============================================================================
Name        : main.cpp
Author      : Junxuan Kao
Version     : 0.0.1
Date         :
Copyright   :
License  :
Description : main definition
===============================================================================
                                        History
---------+---------+--------------------------------------------+-------------
DATE     |  VERSION |   DESCRIPTIONS                             |  By
---------+---------+--------------------------------------------+-------------
19/08/07   0.0.1     Implement for EVB                           Junxuan Kao
===============================================================================
*/

/*
TODO: insert other include files here
*/
#include "mbed.h"
#include "Global.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include "Application.h"

/*
TODO: insert functions here
*/
void Main_Initialize(void)
{        
    EVB_INITIAL_UARTSERVICE();      //Initial UART Service

    ThisThread::sleep_for(5ms);

    pc->Print("\n(System Initializing...)\n");
}

/*
Code entery
*/
int main()
{
    Main_Initialize();

    Application_main();
}
