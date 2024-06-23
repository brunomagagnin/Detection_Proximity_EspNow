#ifndef SETUP_H
#define SETUP_H


#include <M5Core2.h>
#include <lvgl.h>

namespace setupLvgl {

void lv_begin();
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);

}

#endif