#ifndef GUI_H
#define GUI_H

#include <lvgl.h>
#include <Arduino.h>
#include "ESP/ESP_Core.h"



namespace screen
{
    void lv_screen();

    void lv_open_msgbox(int index, ESP_Core *EspCore);
    void lv_msgboxClose();
    lv_obj_t *lv_msgboxObj();

    void create_alert();
    void delete_alert();
    void create_company();
    void delete_company();

    bool getIsCreatIcon();
    bool getIsCreatMsgbox();
    bool getAcceptApprouch();
    int getCurrentTab();
    int getIndexCoreRequest();
}
#endif