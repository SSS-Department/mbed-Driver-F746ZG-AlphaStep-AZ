/*
* Menu.cpp
*
*  Created on: 2021/04/16
*      Author: Junxuan Kao
*/

#include "Menu.h"

/* Constructor */
Menu::Menu(string _name)
{      
    m_name = _name;
    item_count = 0;
}

/* Destructor */
Menu::~Menu()
{
    
}

uint16_t Menu::wait_for_UserInput(void)
{
    uint16_t index;
    while (true)//(pc->writable() == true)
    {
        pc->Print("Select:/> ");
        
        index = pc->GetInteger();

        if (index < 0 || index > item_count)
        {
            //Do Nothing
        }
        else
        {
            pc->Print("\n");
            break;
        }
    }

    return index;
}

int Menu::show(void)
{
    nemuItem tmp;
    int user_selected;
    int roll = 0;
    list<nemuItem>::iterator it;
    
    pc->Print("----------------- %s ------------------\n", m_name.c_str());
    for(it = items.begin(); it != items.end(); it++)
    {
        tmp = *it;
        pc->Print("(%2d)  %s\n", ++roll, tmp.name.c_str());
    }
    user_selected = wait_for_UserInput() - 1;
    
    if(user_selected >= 0)
    {
        if (getItem(user_selected).task != 0)
        { 
            getItem(user_selected).task();
        }
    }
    else
    {
        pc->Print("(Skip)\n");
    }
    return user_selected;
}

void Menu::clear()
{
    items.clear();
}

void Menu::add(string _name, mbed::Callback<void()> _task)
{
    nemuItem _item;
    _item.name = _name;
    _item.task = _task;
    
    items.push_back(_item);
    item_count++;
}

void Menu::add(nemuItem _item)
{
    items.push_back(_item);
    item_count++;
}

nemuItem Menu::getItem(int _index)
{
    nemuItem rpt;
    list<nemuItem>::iterator it = items.begin();
    if(_index < item_count)
    {
        for(int i=0; i<_index; i++)
            ++it;
        rpt = *it;
    }
    
    return rpt;
}
