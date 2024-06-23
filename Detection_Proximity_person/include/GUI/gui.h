#ifndef GUI_H
#define GUI_H

#include <lvgl.h>
namespace screen
{
    void lv_screen();
    void lv_msgbox(int index);

    void create_alert(int index);
    void delete_alert(int index);

    void create_company();
    void delete_company();

    bool getCallMachine();
    void turnOffCallMachine();
    int getCurrentTab();
    
}

#endif