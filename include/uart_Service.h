/*
* uart_Service.hpp
*
*  Created on: 2021/1/13
*      Author: Junxuan Kao
*/

#ifndef UART_SERVICE_H_
#define UART_SERVICE_H_

#include "mbed.h"
#include "RingBuffer.h"
#include <string>

#define START_THREAD 1
#define BUFFER_SIZE 512

class uartService
{
public:
    uartService(PinName tx, PinName rx);
    ~uartService();

    void Init(int _baud, bool _charRpt = false);
    bool writable(void);
    void Print(const char * pu8Str, ...);
    char WaitOneByte(void);
    void ReadOneByte(char *u8tmp);
    bool GetYesNo(void);
    int GetInteger(void);
    
    char GetLastKey();
    string GetLastCommand();
    string GetCommands();
    void CommandClr(void);
    void Reset_UART_Queue(void);
    char Get_UART_Queue_Count(void);
    bool UART_Queue_ReadOne(char* _out);
    void UART_Queue_WriteOne(char _in);    
    void enable(bool _en);

private:
    char uart_buffer[BUFFER_SIZE];   
    bool charRpt;
    void SkipAllBytes(void);
    void CommandCompare(char _inChar);
    void Command_analyzer();

    static void threadStarter(void const *argument);
    void threadWorker();    
    string LastCommand;
    string LastCommand_tmp;
    string str_tmp;
    bool BG_ScanKey_EN;
    char BG_ScanKey_LastKey;
    char LastKey_tmp;    
    RingBuffer<char,32> UART_Queue;

protected:
    //UnbufferedSerial _Serial;
    Thread _Thread;
    Mutex m_mutex;
    Mutex Key_mutex;
    Mutex List_mutex;
    DigitalOut *DebugLED;
};

#endif /* UART_SERVICE_H_ */

