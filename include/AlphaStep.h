/*
===============================================================================
Name        : AlphaStep.h
Author      : Junxuan Kao
Version     : 0.0.1
Date         :
Copyright   :
License  :
Description : AlphaStep Driver
===============================================================================
                                        History
---------+---------+--------------------------------------------+-------------
DATE     |  VERSION |   DESCRIPTIONS                             |  By
---------+---------+--------------------------------------------+-------------
24/03/1   0.0.1     Initial version                             Junxuan Kao
===============================================================================
*/

#include "mbed.h"
#include <ModbusMaster.h>
#include <cstdint>

#ifndef ALPHASTEP_H_
#define ALPHASTEP_H_

#define Debug (1)

#define MODBUS_TX PD_5
#define MODBUS_RX PD_6
#define MODBUS_BAUD (115200)
#define MODBUS_TIMEOUT (20ms)

#define DIRECT_DATA 0x58
#define DIRECT_DATA_NUM 16

enum DDTO : uint32_t
{
    No_0 = 0,
    No_1,
    No_2,
    No_3,
    No_4,
    No_5,
};

enum runMode : uint32_t
{
    None = 0,
    Absolute,
    Relative_cmd,
    Relative_det,
    Continuous_pos = 7,
    Continuous_vel = 16
};

enum trigger : uint32_t
{
    active_None = 0, 
    active_ALL = 1    
};

enum HoldingRegister : uint16_t
{
    Reg_Current_Alarm = 0x0080,
    Reg_Current_Position = 0x00CC,
    Reg_Current_Torque = 0x00D6
};

struct runDataType
{
    uint32_t runDataNumber;
    uint32_t runMode;
    uint32_t pos;
    uint32_t velocity;
    uint32_t accelerate;
    uint32_t decelerate;
    uint32_t runCurrent;
    uint32_t reaction;
};

struct DiagnosisData
{
    uint32_t Current_Alarm;
    uint32_t Current_Position;
    uint32_t Current_Torque;
};

union runData
{
    uint16_t raw[sizeof(runDataType)/sizeof(uint16_t)];
    runDataType items;
};

//template <class T, size_t L>
class AlphaStep
{
public:
    /* Constructor */
    AlphaStep();
    
    /* Destructor */
    ~AlphaStep();

    /* Functions */
    DiagnosisData Diagnosis(uint8_t slaveId);
    void DirectDataManipulationDirective(uint8_t slaveId, uint32_t runDataNumber, uint32_t runMode, uint32_t pos, uint32_t velocity, 
                                    uint32_t accelerate, uint32_t decelerate, uint32_t runCurrent, uint32_t reaction);

    /*Member variable*/
    

private:
    EventQueue queue;
    Thread exeEventThread;
    /*ModBus-RTU Definition*/
    /*
    Template args:
        - `size_t txBufSize` Transmit buffer size
        - `size_t rxBufSize` Receive buffer size
    */
    typedef ModbusMaster<16, 32> MBM;
    //typedef ModbusMaster<64, 64> MBM;

    MBM *modbus;
protected:
    void SWAPEndian(runData *data);
};


#endif /* ALPHASTEP_H_ */
