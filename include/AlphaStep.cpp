/*
* AlphaStep.cpp
*
*  Created on: 2024/03/01
*      Author: Junxuan Kao
*/

#include "AlphaStep.h"
#include "Global.h"
#include <cstdint>
//#include <stdint.h>

/* Constructor */
AlphaStep::AlphaStep()
{
    /*
    Constructor args:
        - `EventQueue* queue` EventQueue used for processing response off IRQ context
        - `PinName txPin` Target TX pin
        - `PinName rxPin` Target RX pin
        - `int baud`: Serial baud rate (also used for Modbus frame delimiting)
        - `uin8_t slaveId`: Modbus slave ID
        - `std::chrono::milliseconds rxTimeout = 50ms`: Modbus response timeout
    */
    modbus = new MBM(&queue, MODBUS_TX, MODBUS_RX, MODBUS_BAUD, MODBUS_TIMEOUT);
    modbus->stx.format(8,SerialBase::Even,1);
    
    exeEventThread.start(callback(&queue, &EventQueue::dispatch_forever));
}

/* Destructor */
AlphaStep::~AlphaStep()
{
    
}

void AlphaStep::SWAPEndian(runData *data)
{
    uint16_t tmp_val;
    for (int count=0; count<sizeof(data->raw)/sizeof(uint16_t); count+=2)
    {
        tmp_val = data->raw[count];
        data->raw[count] = data->raw[count+1];
        data->raw[count+1] = tmp_val;
    }
}

DiagnosisData AlphaStep::Diagnosis(uint8_t slaveId)
{
    DiagnosisData _current;

    //Read Current Alarm
    modbus->readHoldingRegisters(slaveId, HoldingRegister::Reg_Current_Alarm, 2, [this, slaveId, &_current](auto res)
    {
        if(res != MBM::Result::success)
        {
#if Debug
            pc->Print("\t[SlaveID %d] Read Failed(%04X)\n", slaveId, res);
#endif
        }
        else
        {
            uint16_t* reg = modbus->getRegisters();
            _current.Current_Alarm = (reg[0]<<16 & 0xFFFF0000) | reg[1];
        }
    });

    ThisThread::sleep_for(MODBUS_TIMEOUT);

    //Read Current Position
    modbus->readHoldingRegisters(slaveId, HoldingRegister::Reg_Current_Position, 2, [this, slaveId, &_current](auto res)
    {
        if(res != MBM::Result::success)
        {
#if Debug
            pc->Print("\t[SlaveID %d] Read Failed(%04X)\n", slaveId, res);
#endif
        }
        else
        {
            uint16_t* reg = modbus->getRegisters();
            _current.Current_Position = (reg[0]<<16 & 0xFFFF0000) | reg[1];
        }
    });

    ThisThread::sleep_for(MODBUS_TIMEOUT);

    //Read Current Torque
    modbus->readHoldingRegisters(slaveId, HoldingRegister::Reg_Current_Torque, 2, [this, slaveId, &_current](auto res)
    {
        if(res != MBM::Result::success)
        {
#if Debug
            pc->Print("\t[SlaveID %d] Read Failed(%04X)\n", slaveId, res);
#endif
        }
        else
        {
            uint16_t* reg = modbus->getRegisters();
            _current.Current_Torque = (reg[0]<<16 & 0xFFFF0000) | reg[1];
        }
    });

    ThisThread::sleep_for(MODBUS_TIMEOUT);

    return _current;
}

void AlphaStep::DirectDataManipulationDirective(uint8_t slaveId, uint32_t runDataNumber, uint32_t runMode, uint32_t pos, uint32_t velocity, 
                                    uint32_t accelerate, uint32_t decelerate, uint32_t runCurrent, uint32_t reaction)
{
    runData payload;
    payload.items.runDataNumber = runDataNumber;
    payload.items.runMode = runMode;
    payload.items.pos = pos;
    payload.items.velocity = velocity;
    payload.items.accelerate = accelerate;
    payload.items.decelerate = decelerate;
    payload.items.runCurrent = runCurrent;
    payload.items.reaction = reaction;

    SWAPEndian(&payload);

#if Debug
    //Debug
    pc->Print("Ask：");
    for (int count=0; count<sizeof(payload.raw)/sizeof(uint16_t); count++)
    {
        pc->Print("%04X ",payload.raw[count]);
    }
    pc->Print("\n");
#endif

    modbus->writeMultipleRegisters(slaveId, DIRECT_DATA, DIRECT_DATA_NUM, payload.raw ,[this, slaveId](auto res)
    {
        if(res != MBM::Result::success)
        {
#if Debug
            pc->Print("\t[SlaveID %d] writeMultipleRegisters Failed(%04X)\n", slaveId, res);
#endif
        }
    });

    ThisThread::sleep_for(MODBUS_TIMEOUT);
}
