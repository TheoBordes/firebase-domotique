// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.2
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "ui.h"

void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Slider1 = lv_slider_create(ui_Screen2);
    lv_slider_set_value(ui_Slider1, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_Slider1) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Slider1, 100, LV_ANIM_OFF);
    lv_obj_set_width(ui_Slider1, 194);
    lv_obj_set_height(ui_Slider1, 10);
    lv_obj_set_x(ui_Slider1, 1);
    lv_obj_set_y(ui_Slider1, -60);
    lv_obj_set_align(ui_Slider1, LV_ALIGN_CENTER);


    ui_Button3 = lv_btn_create(ui_Screen2);
    lv_obj_set_width(ui_Button3, 100);
    lv_obj_set_height(ui_Button3, 50);
    lv_obj_set_x(ui_Button3, 105);
    lv_obj_set_y(ui_Button3, 87);
    lv_obj_set_align(ui_Button3, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button3, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Button3, lv_color_hex(0x065400), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Button3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_Button3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_Button3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label1 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label1, -3);
    lv_obj_set_y(ui_Label1, -100);
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, "LED PANEL");

    ui_Label5 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_Label5, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label5, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label5, 0);
    lv_obj_set_y(ui_Label5, 7);
    lv_obj_set_align(ui_Label5, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label5, "");

    ui_Slider2 = lv_slider_create(ui_Screen2);
    lv_slider_set_value(ui_Slider2, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_Slider2) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Slider2, 100, LV_ANIM_OFF);
    lv_obj_set_width(ui_Slider2, 194);
    lv_obj_set_height(ui_Slider2, 10);
    lv_obj_set_x(ui_Slider2, 0);
    lv_obj_set_y(ui_Slider2, -11);
    lv_obj_set_align(ui_Slider2, LV_ALIGN_CENTER);


    ui_Slider3 = lv_slider_create(ui_Screen2);
    lv_slider_set_value(ui_Slider3, 0, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_Slider3) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Slider3, 100, LV_ANIM_OFF);
    lv_obj_set_width(ui_Slider3, 194);
    lv_obj_set_height(ui_Slider3, 10);
    lv_obj_set_x(ui_Slider3, 0);
    lv_obj_set_y(ui_Slider3, 40);
    lv_obj_set_align(ui_Slider3, LV_ALIGN_CENTER);


    ui_Label4 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label4, -3);
    lv_obj_set_y(ui_Label4, -45);
    lv_obj_set_align(ui_Label4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label4, "");

    ui_Label6 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_Label6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label6, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label6, -2);
    lv_obj_set_y(ui_Label6, 58);
    lv_obj_set_align(ui_Label6, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label6, "");

    ui_Label15 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_Label15, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label15, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label15, 104);
    lv_obj_set_y(ui_Label15, 88);
    lv_obj_set_align(ui_Label15, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label15, "RELAIS");

    lv_obj_add_event_cb(ui_Slider1, ui_event_Slider1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Button3, ui_event_Button3, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Slider2, ui_event_Slider2, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Slider3, ui_event_Slider3, LV_EVENT_ALL, NULL);

}
