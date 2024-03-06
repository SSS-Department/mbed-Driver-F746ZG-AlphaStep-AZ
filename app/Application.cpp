/*
* TestingAPP.cpp
*
*  Created on: 2019/4/12
*      Author: Junxuan Kao
*/
#include "Application.h"
#include <AlphaStep.h>
#include <cstdint>
#include <cstdio>


/*===================================*/
/*        Define Mac Address         */
/*===================================*/
uint8_t mbed_otp_mac_address(char *mac)
{
    mac[0] = 0x00;
    mac[1] = 0x02;
    mac[2] = 0xF7;
    mac[3] = 0xF0;
    mac[4] = 0x10;  //0x10 for TrackingAntenna, 0x01/0x02 for TrackingCamera
    mac[5] = 0x01;  //TrackingAntenna num. start form 0x01 (0x00 reserved)

    return 1;
};

static char str_buffer_in[256];

AlphaStep motorJX;
enum Axis : uint8_t
{
    z = 1,
    x = 2
};

/*
TODO: insert Function Items here
*/
void diagnosis()
{
    DiagnosisData resZ = motorJX.Diagnosis(Axis::z);
    pc->Print("resZ.Current_Alarm=%d, Current_Position=%d, Current_Torque=%d\n", resZ.Current_Alarm, resZ.Current_Position, resZ.Current_Torque);

    DiagnosisData resX = motorJX.Diagnosis(Axis::x);
    pc->Print("resX.Current_Alarm=%d, Current_Position=%d, Current_Torque=%d\n", resX.Current_Alarm, resX.Current_Position, resX.Current_Torque);
}

void moveMe()
{
    motorJX.DirectDataManipulationDirective(Axis::z, DDTO::No_0, runMode::Absolute, 0, 15000, 60000, 60000, 1000, trigger::active_ALL);
    motorJX.DirectDataManipulationDirective(Axis::x, DDTO::No_0, runMode::Absolute, 0, 15000, 60000, 60000, 1000, trigger::active_ALL);
}

void moveMe2()
{
    motorJX.DirectDataManipulationDirective(Axis::z, DDTO::No_0, runMode::Absolute, 36000, 15000, 60000, 60000, 1000, trigger::active_ALL);
    motorJX.DirectDataManipulationDirective(Axis::x, DDTO::No_0, runMode::Absolute, 36000, 15000, 60000, 60000, 1000, trigger::active_ALL);
}

void moveTo()
{
    pc->Print("(1 = 0.01deg) >");
    int val = pc->GetInteger();

    motorJX.DirectDataManipulationDirective(Axis::z, DDTO::No_0, runMode::Absolute, (uint32_t)val, 15000, 60000, 60000, 1000, trigger::active_ALL);
    motorJX.DirectDataManipulationDirective(Axis::x, DDTO::No_0, runMode::Absolute, (uint32_t)val, 15000, 60000, 60000, 1000, trigger::active_ALL);
}

/*
TODO: insert menu text-functions here
*/
void ShowTitle(void)
{
    /***    Title Information   ***/
    pc->Print("\n");
    pc->Print("*****************************************\n");
    pc->Print("*     Taiwan Innovative Space, Inc.     *\n");
    pc->Print("*               D&D System              *\n");
    pc->Print("*         Firmware Ver: 0.0.0           *\n");
    pc->Print("*      Build: %s %s      *\n", __DATE__, __TIME__);
    pc->Print("*****************************************\n");
    
}

/*
testMain routine
*/
void Application_main(void)
{
    ShowTitle();

    Menu mainMenu("Main Menu");
    mainMenu.add("Diagnosis", &diagnosis);
    mainMenu.add("Move Home", &moveMe);
    mainMenu.add("Move 360", &moveMe2);
    mainMenu.add("Move To", &moveTo);

	while (true)	//Loop foever here
    {
        mainMenu.show();
        thread_sleep_for(200);
    }
}