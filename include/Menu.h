/*
===============================================================================
Name        : Menu.h
Author      : Junxuan Kao
Version     : 0.0.1
Date         :
Copyright   :
License  :
Description : Menu object for text-UI
===============================================================================
                                        History
---------+---------+--------------------------------------------+-------------
DATE     |  VERSION |   DESCRIPTIONS                             |  By
---------+---------+--------------------------------------------+-------------
21/03/16   0.0.1     Initial Version                             Junxuan Kao
===============================================================================
*/


#ifndef MENU_H_
#define MENU_H_

#include <algorithm>
#include <iostream>
#include <list>
#include "mbed.h"
#include "Global.h"

using namespace std;

struct nemuItem
{
    string name;
    mbed::Callback<void()> task;
};

class Menu
{
public:
    Menu(string _name);
    ~Menu();
    
    int show(void);
    int count(){return item_count;}
    void clear();

    void add(string _name, mbed::Callback<void()> _task);
    void add(nemuItem _item);

private:
    list<nemuItem> items;
    int item_count;
    string m_name;
    
    uint16_t wait_for_UserInput(void);
    nemuItem getItem(int _index);

protected:

};

#endif  /*MENU_H_*/