#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include <M5Core2.h>
#include <lvgl.h>

namespace screen
{
    void lv_screen();
    void create_alert();
    void delete_alert(lv_obj_t * e);
}

#endif