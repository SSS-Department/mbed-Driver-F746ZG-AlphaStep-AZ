/*
===============================================================================
Name        : RingBuffer.h
Author      : Junxuan Kao
Version     : 0.0.1
Date         :
Copyright   :
License  :
Description : Name        : RingBuffer.h
 definition
===============================================================================
                                        History
---------+---------+--------------------------------------------+-------------
DATE     |  VERSION |   DESCRIPTIONS                             |  By
---------+---------+--------------------------------------------+-------------
19/07/16   0.0.1     Initial version                             Junxuan Kao
===============================================================================
*/

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include "mbed.h"

template <class T, size_t L>
class RingBuffer
{
public:
    /* Constructor */
    inline RingBuffer(unsigned int size = 64)
            :Queue_Semaphone(0,1)
    {      
        _Queue = new T[L];
        _size = L - 1;
        _rIndex = 0;
        _wIndex = 0;
        
        Queue_Semaphone.release();
    }
    
    /* Destructor */
    ~RingBuffer()
    {
        delete [] _Queue;
    }

    /* Functions */
    int Get_Size(void)
    {
        return (_size + 1);
    }
    
    void Reset_Queue(void)
    {
        _rIndex = 0;
        _wIndex = 0;
    }

    char Get_BufferCount(void)
    {
        if (_wIndex >= _rIndex)
        {
            return (_wIndex - _rIndex);
        }
        else
        {
            return ((_size - _rIndex) + _wIndex);
        }
    }

    void Write_Buffer(T _in)
    {
        _Queue[_wIndex] = _in;
        appendIndex(&_wIndex);
    
        if (_wIndex == _rIndex)
        {
            appendIndex(&_rIndex);
        }
    }
    
    /**
    * You cannot call Write_Buffer_safe() from ISR context.
    * This is for thread-safe design.
    */
    void Write_Buffer_safe(T _in)
    {
        Queue_Semaphone.acquire();
        
        _Queue[_wIndex] = _in;
        appendIndex(&_wIndex);
    
        if (_wIndex == _rIndex)
        {
            appendIndex(&_rIndex);
        }
        
        Queue_Semaphone.release();
    }

    bool Read_Buffer(T *_out)
    {   
        if (_rIndex != _wIndex)
        {
            *_out = _Queue[_rIndex];
            appendIndex(&_rIndex);
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
    * You cannot call Read_Buffer_safe() from ISR context.
    * This is for thread-safe design.
    */
    bool Read_Buffer_safe(T *_out)
    {   
        if (_rIndex != _wIndex)
        {
            Queue_Semaphone.acquire();
            *_out = _Queue[_rIndex];
            appendIndex(&_rIndex);
            Queue_Semaphone.release();
            return true;
        }
        else
        {
            return false;
        }
    }    

private:
    /* Member variable */
    Semaphore Queue_Semaphone;
    T *_Queue;
    int _rIndex;
    int _wIndex;
    int _size;
    
    void appendIndex(int *inIndex)
    {
        *inIndex = *inIndex + 1;            
        if (*inIndex > _size){*inIndex = 0;}
    }
};

#endif /* RINGBUFFER_H_ */

