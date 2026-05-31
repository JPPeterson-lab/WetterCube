#include "ui.h"

lv_obj_t *ui_Screen3    = NULL;
lv_obj_t *ui_LabelUV      = NULL;
lv_obj_t *ui_LabelSunrise = NULL;
lv_obj_t *ui_LabelSunset  = NULL;

static lv_obj_t *make_header(lv_obj_t *parent, const char *text, lv_coord_t y_ofs)
{
    lv_obj_t *ta = lv_textarea_create(parent);
    lv_obj_set_width(ta, 239);
    lv_obj_set_height(ta, 31);
    lv_obj_set_x(ta, 0);
    lv_obj_set_y(ta, y_ofs);
    lv_obj_set_align(ta, LV_ALIGN_CENTER);
    lv_textarea_set_text(ta, text);
    lv_obj_clear_flag(ta, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                          LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_scrollbar_mode(ta, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_color(ta, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ta, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ta, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ta, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ta, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ta, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ta, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ta, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    return ta;
}

static lv_obj_t *make_value(lv_obj_t *parent, const char *text, lv_coord_t y_ofs)
{
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_set_width(lbl, LV_SIZE_CONTENT);
    lv_obj_set_height(lbl, LV_SIZE_CONTENT);
    lv_obj_set_x(lbl, 0);
    lv_obj_set_y(lbl, y_ofs);
    lv_obj_set_align(lbl, LV_ALIGN_CENTER);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lbl, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    return lbl;
}

void ui_Screen3_screen_init(void)
{
    ui_Screen3 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Screen3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Row 1: UV-Index
    make_header(ui_Screen3, "UV-Index (Max)", -92);
    ui_LabelUV = make_value(ui_Screen3, "--", -57);

    // Row 2: Sonnenaufgang
    make_header(ui_Screen3, "Sonnenaufgang", -18);
    ui_LabelSunrise = make_value(ui_Screen3, "--:--", 17);

    // Row 3: Sonnenuntergang
    make_header(ui_Screen3, "Sonnenuntergang", 52);
    ui_LabelSunset = make_value(ui_Screen3, "--:--", 87);
}

void ui_Screen3_screen_destroy(void)
{
    if (ui_Screen3) lv_obj_del(ui_Screen3);
    ui_Screen3    = NULL;
    ui_LabelUV      = NULL;
    ui_LabelSunrise = NULL;
    ui_LabelSunset  = NULL;
}
