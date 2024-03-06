/*
* uart_Service.cpp
*
*  Created on: 2021/1/13
*      Author: Junxuan Kao
*/

#include "uart_Service.h"

UnbufferedSerial* _Serial;

uartService::uartService(PinName tx, PinName rx)
  : //_Serial(tx, rx, MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE),
    _Thread(osPriorityLow, 512, NULL, "scankey_thread")
{
    _Serial = new UnbufferedSerial(tx, rx, MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE);
    UART_Queue.Reset_Queue();
    _Thread.start(callback(uartService::threadStarter, this));
    BG_ScanKey_EN = false;
};

uartService::~uartService()
{
    //Do Nothing
}

FileHandle *mbed::mbed_override_console(int fd)
{
    return _Serial;
}

int a2i(char *s)
{
    int num = 0;
    int sign = 1;

    if (*s == '-')
    {
        sign = -1;
        s++;
    }
    else
    {
        //DoNothing
    }

    while (*s)
    {
        if (isdigit(*s))
        {
            num = ((*s) - '0') + num * 10;
            s++;
        }
        else
        {
            s++;
        }
    }

    return num*sign;
}

void uartService::Init(int _baud, bool _charRpt)
{
    _Serial->baud(_baud);
    charRpt = _charRpt;    
    _Thread.flags_set(START_THREAD);
}

bool uartService::writable(void)
{
    return _Serial->writable();
}

void uartService::Print(const char* format, ...)
{
    m_mutex.lock();

    va_list args;

    va_start(args, format);
    vsnprintf(uart_buffer, BUFFER_SIZE, format, args);
    va_end(args);

    //_Serial->write(uart_buffer, sizeof(uart_buffer));
    printf(uart_buffer);
    fflush(stdout);
    
    m_mutex.unlock();
}

char uartService::WaitOneByte(void)
{
    char u8Tmp = 0;

    m_mutex.lock();
    while (true)
    {
        if (_Serial->readable() == true)
        {
            _Serial->read(&u8Tmp, 1);

            if (u8Tmp > 0 && u8Tmp != 16 && u8Tmp != 33  && u8Tmp != 165)
            {
                break;
            }
        }

        thread_sleep_for(10);
    }
    m_mutex.unlock();

    return u8Tmp;
}

void uartService::SkipAllBytes(void)
{
    char tmp;
    if (_Serial->readable())
    {
        m_mutex.lock();
        while (_Serial->readable())
        {
            _Serial->read(&tmp,1);
        }
        m_mutex.unlock();
    }
}

void uartService::ReadOneByte(char *u8tmp)
{
    fflush(stdout);
    *u8tmp = WaitOneByte();
}

bool uartService::GetYesNo(void)
{
    bool rpt = false;
    char u8Sel = 0;
    BG_ScanKey_EN = false;

    SkipAllBytes();

    while (true)
    {
        Print(" [Y/N]: ");
        ReadOneByte(&u8Sel);

        if (isalpha(u8Sel))
        {
            if ((u8Sel == 121) || (u8Sel == 89))        // 'y' or 'Y'
            {
                Print("Y\n");
                rpt = true;
                break;                
            }
            else if ((u8Sel == 110) || (u8Sel == 78))   // 'n' or 'N'
            {
                Print("N\n");
                rpt = false;
                break;
            }
            else
            {
                //Do Nothing
            }
        }
        else
        {
            if (u8Sel == 13)    // if (u8Sel == "\r")
            {
                Print("Y\n");
                rpt = true;
                break;
            }
            else
            {
                //Do Nothing
            }
        }
        thread_sleep_for(10);
    }

    BG_ScanKey_EN = true;

    return rpt;
}

int uartService::GetInteger(void)
{
    char u8Array[5 + 2];
    uint8_t i;
    bool is_Negative = false;
    BG_ScanKey_EN = false;
    memset(&u8Array, 0x00, sizeof(u8Array));

    SkipAllBytes();

    for (i = 0; i < sizeof(u8Array); i++)
    {
        ReadOneByte(&u8Array[i]);

        if (charRpt == true && u8Array[i] != '\b')
        {
            Print("%c", u8Array[i]);
        }

        if (!is_Negative)
        {
            if (i == (sizeof(u8Array) - 2))
            {
                if (u8Array[i] == 13 || u8Array[i] == '\b' || u8Array[i] == 47) //Check if the final byte is '\r' or '\b' or '/'
                {
                    //DoNothing
                }
                else
                {
                    Print("\n\tIncorrect Number Type! (Too Long), Input Again: ");
                    i = 0xFF;
                    memset(&u8Array, 0x00, sizeof(u8Array));
                    continue;
                }
            }
        }
        else    //is_Negative
        {
            if (i == (sizeof(u8Array) - 1))
            {
                if (u8Array[i] == 13 || u8Array[i] == '\b') //Check if the final byte is '\r' or '\b'
                {
                    //DoNothing
                }
                else
                {
                    Print("\n\tIncorrect Number Type! (Too Long), Input Again: ");
                    i = 0xFF;
                    memset(&u8Array, 0x00, sizeof(u8Array));
                    continue;
                }
            }
        }

        if (isdigit(u8Array[i]) == false)
        {
            if (u8Array[i] == '-')
            {
                if (i == 0)
                {
                    is_Negative = true;
                    continue;
                }
                else
                {
                    Print("\n\tIncorrect Number Type! (%d), Input Again: ", u8Array[i]);
                    i = 0xFF;
                    memset(&u8Array, 0x00, sizeof(u8Array));
                    continue;
                }
            }

            if (u8Array[i] == '\b')
            {
                if ((i - 1) >= 0)
                {
                    if (charRpt == true)
                    {
                        Print("\b");
                    }
                }

                i -= 2;

                if (i  >= (sizeof(u8Array) - 1))
                {
                    i = 0xFF;
                }

                continue;
            }
            else if (u8Array[i] == 13) //Get '\r'
            {
                break;
            }
            else if (u8Array[i] == 47) //Get '/'
            {
                i--;
                continue;
            }
            else
            {
                Print("\n\tIncorrect Number Type! (%d), Input Again: ", u8Array[i]);
                i = 0xFF;
                memset(&u8Array, 0x00, sizeof(u8Array));
                continue;
            }
        }
    }

    if (charRpt == true)
    {
        Print("\n");
    }

    BG_ScanKey_EN = true;
    
    return a2i(u8Array);
}

void uartService::threadStarter(void const *argument)
{
    uartService *instance = (uartService*)argument;
    instance->threadWorker();
}

void uartService::CommandClr(void)
{
    Key_mutex.lock();
    LastCommand = "";
    LastCommand_tmp = "";
    Reset_UART_Queue();
    Key_mutex.unlock();
}

void uartService::Reset_UART_Queue(void)
{
    UART_Queue.Reset_Queue();
}

char uartService::Get_UART_Queue_Count(void)
{
    return UART_Queue.Get_BufferCount();
}

void uartService::UART_Queue_WriteOne(char _in)
{
    UART_Queue.Write_Buffer(_in);
}

bool uartService::UART_Queue_ReadOne(char *_out)
{
    return UART_Queue.Read_Buffer(_out);
}

void uartService::enable(bool _en)
{
    BG_ScanKey_EN = _en;
}

void uartService::threadWorker()
{
    //Thread Initial
    BG_ScanKey_EN = true;
    BG_ScanKey_LastKey = 0x00;
    LastKey_tmp = 0x00;
    LastCommand = "";

    rtos::ThisThread::flags_wait_all(START_THREAD);

    while (true)
    {
        if (BG_ScanKey_EN == true)
        {
            if (_Serial->readable())
            {
                _Serial->read(&BG_ScanKey_LastKey, 1);
                UART_Queue_WriteOne(BG_ScanKey_LastKey);
                CommandCompare(BG_ScanKey_LastKey);
            }
            else
            {
                rtos::ThisThread::sleep_for(10);
            }
        }
        else
        {
            Key_mutex.lock();
            if (LastCommand_tmp != "")
            {
                LastCommand_tmp = "";
            }
            Key_mutex.unlock();

            rtos::ThisThread::sleep_for(50);
        }
    }
}

char uartService::GetLastKey()
{
    Key_mutex.lock();

    if (Get_UART_Queue_Count() > 0)
    {
        if (UART_Queue_ReadOne(&LastKey_tmp) == true)
        {
            //DoNothing
        }
        else    //Read Failed
        {
            LastKey_tmp = 0x00;
        }
    }
    else
    {
        LastKey_tmp = 0x00;
    }

    Key_mutex.unlock();

    return LastKey_tmp;
}

string uartService::GetLastCommand()
{
    string str;

    Key_mutex.lock();

    if (LastCommand != "")
    {
        str = LastCommand;
        LastCommand = "";
    }
    else
    {
        str = "";
    }

    Key_mutex.unlock();

    return str;
}

void uartService::CommandCompare(char _inChar)
{
    /*Implement for runtime command input*/
    if ((_inChar >= 32 && _inChar <= 96) || (_inChar >= 97 && _inChar <= 126))
    {
        LastCommand_tmp += (char)_inChar;

        //Print("Debug: LastCommand_tmp-> %s\n",LastCommand_tmp);
    }
    else if (_inChar == 27) //ESC = Reset the LastCommand_tmp
    {
        LastCommand_tmp = "";
    }
    else if (_inChar == 8)
    {
        if (LastCommand_tmp.length() > 0)
        {
            LastCommand_tmp = LastCommand_tmp.substr(0, LastCommand_tmp.length() - 1);
        }
    }
    else if (_inChar == 13)
    {
        Command_analyzer();
    }
    /*End of the runtime command input*/
}

void uartService::Command_analyzer()
{
    Key_mutex.lock();

    LastCommand = LastCommand_tmp;
    LastCommand_tmp = "";

    Key_mutex.unlock();

    if (LastCommand.length() > 0)
    {
        //Print("Debug: LastCommand-> %s\n",LastCommand);

        if (LastCommand == "%SystemReset%")
        {
            NVIC_SystemReset();
        }
    }
}

