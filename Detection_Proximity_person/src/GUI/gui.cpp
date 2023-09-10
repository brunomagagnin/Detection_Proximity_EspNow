#include "gui.h"

LV_IMG_DECLARE(Logo);
lv_obj_t *imgLogo;

LV_IMG_DECLARE(Icone);
lv_obj_t *imgTab1;
lv_obj_t *imgTab2;

lv_obj_t *tabview;
lv_obj_t *tab1;
lv_obj_t *tab2;

bool callMachine = false;
int currentTab = 0;

namespace screen
{
    static lv_obj_t *slider_label;

    static void event_tabButton(lv_event_t *e)
    {
        lv_obj_t *btns = lv_event_get_current_target(e);

        currentTab = lv_btnmatrix_get_selected_btn(btns);
    }

    void tabview_screen(void)
    {
        tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
        lv_obj_set_style_bg_color(tabview, lv_color_black(), 0);

        tab1 = lv_tabview_add_tab(tabview, "Maquina 1");
        tab2 = lv_tabview_add_tab(tabview, "Maquina 2");

        lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
        lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
        lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);

        /*
        lv_obj_t *label = lv_label_create(tab1);
        lv_obj_t *label2 = lv_label_create(tab2);
         lv_label_set_text(label, " ");
        lv_label_set_text(label2, " ");
        label = lv_label_create(tab1);
        label2 = lv_label_create(tab2);
        */

        lv_obj_add_event_cb(tab_btns, event_tabButton, LV_EVENT_CLICKED, NULL);
    }

    void create_alert(int index)
    {
        if (index == 0)
        {
            imgTab1 = lv_img_create(tab1);
            lv_img_set_src(imgTab1, &Icone);
            lv_obj_align(imgTab1, LV_ALIGN_CENTER, 0, -15);
        }

        if (index == 1)
        {
            imgTab2 = lv_img_create(tab2);
            lv_img_set_src(imgTab2, &Icone);
            lv_obj_align(imgTab2, LV_ALIGN_CENTER, 0, -15);
        }
    }

    void delete_alert(int index)
    {
        if (index == 0)
            lv_obj_del(imgTab1);
        if (index == 1)
            lv_obj_del(imgTab2);
    }

    static void event_button(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        if (code == LV_EVENT_CLICKED)
        {
            callMachine = true;
        }
    }

    void lv_btn()
    {
        static lv_style_t style_v;
        lv_style_init(&style_v);

        lv_style_set_radius(&style_v, 3);

        lv_style_set_bg_opa(&style_v, LV_OPA_100);
        lv_style_set_bg_color(&style_v, lv_palette_main(LV_PALETTE_BLUE));
        lv_style_set_bg_grad_color(&style_v, lv_palette_darken(LV_PALETTE_BLUE, 2));
        lv_style_set_bg_grad_dir(&style_v, LV_GRAD_DIR_VER);

        lv_style_set_border_opa(&style_v, LV_OPA_40);
        lv_style_set_border_width(&style_v, 2);
        lv_style_set_border_color(&style_v, lv_palette_main(LV_PALETTE_GREY));

        lv_style_set_outline_opa(&style_v, LV_OPA_COVER);
        lv_style_set_outline_color(&style_v, lv_palette_main(LV_PALETTE_BLUE));

        lv_style_set_text_color(&style_v, lv_color_white());
        lv_style_set_pad_all(&style_v, 10);

        // pressed style
        static lv_style_t style_pr;
        lv_style_init(&style_pr);

        lv_style_set_outline_width(&style_pr, 30);
        lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

        lv_style_set_translate_y(&style_pr, 5);
        lv_style_set_shadow_ofs_y(&style_pr, 3);
        lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
        lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));

        // Add a transition to the outline
        static lv_style_transition_dsc_t trans;

        static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA};
        lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

        lv_style_set_transition(&style_pr, &trans);

        lv_obj_t *btn1 = lv_btn_create(lv_scr_act());

        lv_obj_add_style(btn1, &style_v, 0);
        lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
        lv_obj_set_size(btn1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_center(btn1);

        lv_obj_t *label1 = lv_label_create(btn1);
        lv_label_set_text(label1, "REQUISITAR");
        lv_obj_center(label1);
        lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 90);
        lv_obj_add_event_cb(btn1, event_button, LV_EVENT_CLICKED, NULL);
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

    void lv_screen()
    {
        tabview_screen();
        lv_btn();
    }

    bool getCallMachine()
    {
        return callMachine;
    }

    void setCallMachine()
    {
        callMachine = false;
    }

    int lv_current_tab()
    {
        return currentTab;
    }

}
