/**
 * @file reset.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Reset screens
 * @version 2.0.1
 * @date 2022-09-07
 *
 * @copyright Copyright (c) 2023
 *
 * 

 .S_sSSs      sSSs    sSSs    sSSs  sdSS_SSSSSSbs          sSSs  
.SS~YS%%b    d%%SP   d%%SP   d%%SP  YSSS~S%SSSSSP         d%%SP  
S%S   `S%b  d%S'    d%S'    d%S'         S%S             d%S'    
S%S    S%S  S%S     S%|     S%S          S%S             S%S     
S%S    d*S  S&S     S&S     S&S          S&S             S&S     
S&S   .S*S  S&S_Ss  Y&Ss    S&S_Ss       S&S             S&S     
S&S_sdSSS   S&S~SP  `S&&S   S&S~SP       S&S             S&S     
S&S~YSY%b   S&S       `S*S  S&S          S&S             S&S     
S*S   `S%b  S*b        l*S  S*b          S*S             S*b     
S*S    S%S  S*S.      .S*P  S*S.         S*S             S*S.    
S*S    S&S   SSSbs  sSS*S    SSSbs       S*S              SSSbs  
S*S    SSS    YSSP  YSS'      YSSP       S*S        SS     YSSP  
SP                                       SP        S%%S          
Y                                        Y          SS           
                                                                 

 */

#include "reset.h"

LV_IMG_DECLARE(spinner_check)

static void smart_ring_ui_reset_cancel_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_update_state(STATE_36);
    }
}

static void smart_ring_ui_reset_device_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_get_controller()->flags.flag.reset_device         = true;
    }
}

static void smart_ring_ui_reset_wifi_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_update_state(STATE_39);
        smart_ring_ui_get_controller()->flags.flag.reset_wifi = true;
    }
}

static void smart_ring_ui_reset_wifi_failed_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_update_state(STATE_36);
    }
}

void reset_create_device_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
    create_header(parent, "Reiniciar", ui_controller);

    static lv_style_t style_blue_text;
    lv_style_init(&style_blue_text);
    lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

    lv_obj_t *label = lv_label_create(parent, NULL);
    lv_label_set_text(label, "Pretende reiniciar o");
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &ui_controller->styles.font_16_normal);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN,LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -45);

    label = lv_label_create(parent, NULL);
    lv_label_set_text(label, "Dispositivo");
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &ui_controller->styles.font_26_normal);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /********************
     *  VALIDATE BUTTON
     ********************/
    lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(btn_validate, smart_ring_ui_reset_device_handler);
    lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &ui_controller->styles.blue_button);
    lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
    lv_obj_align(btn_validate, NULL, LV_ALIGN_OUT_BOTTOM_MID, -80, -60);

    lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
    lv_label_set_text(label_btn, LV_SYMBOL_OK);

    /******************
     *  CANCEL BUTTON
     ******************/
    static lv_style_t style_btn_red;
    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);

    lv_obj_t *btn_cancel = lv_btn_create(parent, btn_validate);
    lv_obj_set_event_cb(btn_cancel, smart_ring_ui_reset_cancel_handler);
    lv_obj_add_style(btn_cancel, LV_BTN_PART_MAIN, &style_btn_red);
    lv_obj_align(btn_cancel, NULL, LV_ALIGN_OUT_BOTTOM_MID, 80, -60);

    label_btn = lv_label_create(btn_cancel, NULL);
    lv_label_set_text(label_btn, LV_SYMBOL_CLOSE);
}

void reset_create_wifi_screen(
    lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

    create_header(parent, "Reiniciar", ui_controller);

    static lv_style_t style_blue_text;
    lv_style_init(&style_blue_text);
    lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

    lv_obj_t *label = lv_label_create(parent, NULL);
    lv_label_set_text(label, "Pretende reiniciar o");
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &ui_controller->styles.font_16_normal);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -45);

    label = lv_label_create(parent, NULL);
    lv_label_set_text(label, "WiFi");
    lv_obj_add_style(label,   LV_LABEL_PART_MAIN, &ui_controller->styles.font_26_normal);
    lv_obj_add_style(label,   LV_LABEL_PART_MAIN, &style_blue_text);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /********************
     *  VALIDATE BUTTON
     ********************/
    lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(btn_validate, smart_ring_ui_reset_wifi_handler);
    lv_obj_add_style(btn_validate,   LV_BTN_PART_MAIN, &ui_controller->styles.blue_button);
    lv_obj_set_size(btn_validate,    SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
    lv_obj_align(btn_validate, NULL, LV_ALIGN_OUT_BOTTOM_MID, -80, -60);

    lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
    lv_label_set_text(label_btn, LV_SYMBOL_OK);

    /******************
     *  CANCEL BUTTON
     ******************/
    static lv_style_t style_btn_red;
    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);

    lv_obj_t *btn_cancel = lv_btn_create(parent, btn_validate);
    lv_obj_set_event_cb(btn_cancel, smart_ring_ui_reset_cancel_handler);
    lv_obj_add_style(btn_cancel,   LV_BTN_PART_MAIN, &style_btn_red);
    lv_obj_align(btn_cancel, NULL, LV_ALIGN_OUT_BOTTOM_MID, 80, -60);

    label_btn = lv_label_create(btn_cancel, NULL);
    lv_label_set_text(label_btn, LV_SYMBOL_CLOSE);
}

void reset_create_progress_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

    create_header(parent, "Reiniciar", ui_controller);

    lv_obj_t *spinner = lv_spinner_create(parent, NULL);
    lv_obj_set_size(spinner, 70, 70);
    lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_local_line_width(spinner, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, 4);
    lv_obj_set_style_local_line_color(spinner, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
    lv_obj_set_style_local_line_width(spinner, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 4);
    lv_obj_set_style_local_line_color(spinner, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
    lv_spinner_set_arc_length(spinner, 60);

    lv_obj_t *label = lv_label_create(parent, NULL);
    lv_label_set_text(label, "A reiniciar");
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &ui_controller->styles.font_18_bold);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 50);

    label = lv_label_create(parent, NULL);
    lv_label_set_text(label, "Por favor aguarde");
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &ui_controller->styles.font_18_normal);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 80);
}

void reset_create_finished_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller, bool success) {

    create_header(parent, "Reiniciar", ui_controller);

    lv_obj_t *arc = lv_spinner_create(parent, NULL);
    lv_obj_set_size(arc, 80, 80);
    lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_local_line_width(arc, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, 4);
    lv_obj_set_style_local_line_color(arc, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
    lv_obj_set_style_local_line_width(arc, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 4);
    lv_obj_set_style_local_line_color(arc, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
    lv_spinner_set_arc_length(arc, 359);

    if (success) {
        lv_obj_t *img_arc = lv_img_create(arc, NULL);
        lv_img_set_src(img_arc, &spinner_check);
        lv_img_set_zoom(img_arc, 180);
        lv_obj_align(img_arc, NULL, LV_ALIGN_CENTER, 0, 0);
    } 
    else {
        lv_obj_t *label_arc = lv_label_create(arc, NULL);
        lv_label_set_text(label_arc, LV_SYMBOL_CLOSE);
        lv_obj_set_style_local_text_font(label_arc,  LV_LABEL_PART_MAIN, LV_STATE_DEFAULT,  &lv_font_montserrat_24);
        lv_obj_set_style_local_text_color(label_arc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_obj_align(label_arc, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_local_line_color(arc, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED);
    }

    lv_obj_t *label = lv_label_create(parent, NULL);
    lv_label_set_text(label, success ? "WiFi reiniciado com sucesso" : "Erro ao reiniciar WiFi");
    lv_obj_add_style(label, LV_LABEL_PART_MAIN,  &ui_controller->styles.font_18_bold);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 35);

    /********************
     *  VALIDATE BUTTON
     ********************/
    lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(btn_validate, success ? smart_ring_ui_reset_device_handler : smart_ring_ui_reset_wifi_failed_handler);
    lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &ui_controller->styles.blue_button);
    lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
    lv_obj_align(btn_validate, NULL, LV_ALIGN_OUT_BOTTOM_MID, 00, -60);

    lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
    lv_label_set_text(label_btn, LV_SYMBOL_OK);
}

/* END OF FILE */