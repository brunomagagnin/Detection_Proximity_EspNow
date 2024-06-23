#include "GUI/GUI.h"

/*==================================================================================================
--------------------------------------------  VARIABLES  -------------------------------------------
====================================================================================================*/

LV_IMG_DECLARE(Icone);
LV_IMG_DECLARE(Logo);

// TabView
lv_obj_t *tabview;
lv_obj_t *tab1;

// Message Box
int indexRequest = false;
lv_obj_t *mbox;
lv_obj_t *eventMbox;
bool msgoBoxCreate = false;
bool acceptApprouch = false;

// Image
lv_obj_t *imgTab;
lv_obj_t *imgLogo;
bool iconCreated = false;

ESP_Core *esp;


/*==================================================================================================
--------------------------------------------   STRUCT    -------------------------------------------
====================================================================================================*/
typedef struct {
    int index;
    bool acceptApproach;
} StatusStruct;


StatusStruct statusStruct = { 0, false };
/*==================================================================================================
--------------------------------------------TABVIEW-------------------------------------------------
====================================================================================================*/
namespace screen
{
    void tabview_screen()
    {
        tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
        lv_obj_set_style_bg_color(tabview, lv_color_black(), 0);

        tab1 = lv_tabview_add_tab(tabview, "ALERTAS");

        lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
        lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
        lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);
    }

    /*==================================================================================================
    -------------------------------------------- LABEL -------------------------------------------------
    ====================================================================================================*/
    void label_msg(String name)
    {
        lv_obj_set_style_bg_color(tabview, lv_palette_darken(LV_PALETTE_RED, 4), 0);

        String cName = "#0000ff " + name + "#";
        int stringLength = cName.length() + 1;
        char char_array[stringLength];

        cName.toCharArray(char_array, stringLength);

        lv_obj_t *label;
        label = lv_label_create(tab1);

        lv_label_set_recolor(label, true);
        lv_label_set_text(label, char_array);
        lv_obj_set_style_text_font(label, LV_FONT_MONTSERRAT_10, 0);
        lv_obj_set_width(label, 150);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, -40);
    }

    /*==================================================================================================
    -------------------------------------------- IMAGES -----------------------------------------------
    ====================================================================================================*/
    void create_alert()
    {
        imgTab = lv_img_create(tab1);
        lv_img_set_src(imgTab, &Icone);
        lv_obj_align(imgTab, LV_ALIGN_CENTER, 0, -15);
    }

    void delete_alert()
    {
        iconCreated = false;
        lv_obj_del(imgTab);
        statusStruct.acceptApproach = false;
    }

    void create_company()
    {
        imgLogo = lv_img_create(lv_scr_act());
        lv_img_set_src(imgLogo, &Logo);
        lv_obj_align(imgLogo, LV_ALIGN_CENTER, 0, 0);
    }

    void delete_company()
    {
        lv_obj_del(imgLogo);
    }

    /*==================================================================================================
    ------------------------------------------ MESSAGE BOX ---------------------------------------------
    ====================================================================================================*/

    static void event_m(lv_event_t *e)
    {
        eventMbox = lv_event_get_current_target(e);
        LV_LOG_USER("Button %s clicked", lv_msgbox_get_active_btn_text(obj));
        String text = lv_msgbox_get_active_btn_text(eventMbox);
        if (text == "Aceitar")
        {
            statusStruct.acceptApproach = true;
            msgoBoxCreate = false;
            lv_msgbox_close(mbox);
            esp->setRequestApproach(false);
            create_alert();
        }
        if (text == "Recusar")
        {
            acceptApprouch = false;
            esp->setRequestApproach(false);
            msgoBoxCreate = false;
            lv_msgbox_close(mbox);
        }
    }

    void lv_open_msgbox(int index, ESP_Core *EspCore)
    {
        static const char *btns[] = {"Aceitar", "Recusar", ""};
        mbox = lv_msgbox_create(NULL, " ", "Requisitado aproximacao de operador.", btns, false);
        lv_obj_add_event_cb(mbox, event_m, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_center(mbox);
        msgoBoxCreate = true;
        statusStruct.index = index;
        esp = EspCore;
    }

    void lv_msgboxClose()
    {
        msgoBoxCreate = false;
        lv_msgbox_close(mbox);
    }
    /*==================================================================================================
    ---------------------------------------- CALL FUNCTION ---------------------------------------------
    ====================================================================================================*/

    lv_obj_t *lv_msgboxObj()
    {
        return mbox;
    }

    void lv_screen()
    {
        tabview_screen();
    }

     bool getIsCreatIcon()
    {
        return iconCreated;
    }

    bool getIsCreatMsgbox()
    {
        return msgoBoxCreate;
    }

    bool getAcceptApprouch()
    {
        return statusStruct.acceptApproach;
    }
    int getIndexCoreRequest(){
        return statusStruct.index;
    }

}
