/*
* Global.cpp
*
*  Created on: 2019/08/08
*      Author: Junxuan Kao
*/

#include "Global.h"

DigitalOut LD1(LED1);
DigitalOut LD2(LED2);
DigitalOut LD3(LED3);
uartService* pc;

/*
TODO: insert global functions here
*/
bool hexChk(string &str)
{
    bool ret = false;

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    if (str.find("0x",0) == 0)
    {
        str = str.substr(2,str.length()-2);

        ret = true;
    }
    return ret;
}

float Stofloat(string str)
{
    double ret = 3.3;

    ret = atof(str.c_str());

    return (float)ret;
}

int StoNum(string str)
{
    int ret = 0;

    if (hexChk(str))
    {
        ret = strtoul(str.c_str(),NULL,16);
    }
    else
    {
        ret = atoi(str.c_str());
    }

    return ret;
}

void EVB_INITIAL_UARTSERVICE(void)
{
    pc = new uartService(USBTX, USBRX);
    pc->Init(BaudRate, true);
}
