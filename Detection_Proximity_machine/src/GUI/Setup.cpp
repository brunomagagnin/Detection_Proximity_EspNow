#include "GUI/Setup.h"

/**********************
         OBJECT
 **********************/
TFT_eSPI tft = TFT_eSPI();



/**********************
 *  STATIC VARIABLES
 **********************/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];


namespace setupLvgl
{
  // Display flushing
  void my_disp_flush(lv_disp_drv_t *disp,
                     const lv_area_t *area, lv_color_t *color_p)
  {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
  }

  // Read the touchpad
  void my_touchpad_read(lv_indev_drv_t *indev_driver,
                        lv_indev_data_t *data)
  {
    TouchPoint_t pos = M5.Touch.getPressPoint();
    bool touched = (pos.x == -1) ? false : true;
    if (!touched)
    {
      data->state = LV_INDEV_STATE_REL;
    }
    else
    {
      data->state = LV_INDEV_STATE_PR;
      data->point.x = pos.x;
      data->point.y = pos.y;
    }
  }

  void lv_begin()
  {
    lv_init();
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, screenWidth * screenHeight / 10);

    // Display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    // Initialize the input device driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
  }
}