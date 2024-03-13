#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include <M5Core2.h>
#include <lvgl.h>
namespace screen
{
    void lv_screen();
    void lv_msgbox(int index);

    void create_alert(int index);
    void delete_alert(int index);

    bool getCallMachine();
    void setCallMachine();

    int lv_current_tab();
    void create_company();
    void delete_company();
}

#endif