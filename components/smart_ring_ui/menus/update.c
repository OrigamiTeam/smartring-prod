/**
 * @file update.c
 * @author Ayinde Olayiwola (olay@quleap.com)
 * @brief Firmware update screens
 * @version 2.0.1
 * @date 2022-09-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "update.h"

LV_IMG_DECLARE(spinner_check)
LV_IMG_DECLARE(icon_logout)

static lv_obj_t *label_arc;
static lv_obj_t *spinner;


static void smart_ring_ui_update_cancel_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_update_state(STATE_28);
        smart_ring_ui_get_controller()->flags.flag.cancel_update = true;
    }
}


void smart_ring_ui_update_download_started() {
    lv_obj_add_style(label_arc, LV_LABEL_PART_MAIN, &smart_ring_ui_get_controller()->styles.font_24_normal);
    lv_spinner_set_arc_length(spinner, 60);
    lv_label_set_text(label_arc, "0%");
}


void smart_ring_ui_update_change_percentage(int percentage) {
    lv_label_set_text_fmt(label_arc, "%d%%", percentage);
}


void update_create_firmware_progress_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
    create_header(parent, "Atualizar", ui_controller);

    spinner = lv_spinner_create(parent, NULL);
    lv_obj_set_size(spinner, 130, 130);
    lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_local_line_width(spinner, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, 4);
    lv_obj_set_style_local_line_color(spinner, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
    lv_obj_set_style_local_line_width(spinner, LV_SPINNER_PART_INDIC,LV_STATE_DEFAULT, 4);
    lv_obj_set_style_local_line_color(spinner, LV_SPINNER_PART_INDIC,LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
    lv_spinner_set_arc_length(spinner, 359);

    label_arc = lv_label_create(spinner, NULL);
    lv_label_set_text(label_arc, LV_SYMBOL_CLOSE);
    lv_obj_add_style(label_arc, LV_LABEL_PART_MAIN, &ui_controller->styles.font_16_normal);
    lv_obj_set_style_local_text_color(label_arc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_auto_realign(label_arc, true);
    lv_label_set_text(label_arc, "A conectar");
    lv_obj_align_origo(label_arc, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(parent, NULL);
    lv_label_set_text(label, "A atualizar");
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &ui_controller->styles.font_18_bold);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 80);
}


static void smart_ring_ui_update_failed_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_update_state(STATE_28);
    }
}


static void smart_ring_ui_update_complete_handler(lv_obj_t *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        esp_restart();
    }
}


void update_create_firmware_finished_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller, bool success) {
    create_header(parent, "Atualizar", ui_controller);

    lv_obj_t *arc = lv_spinner_create(parent, NULL);
    lv_obj_set_size(arc, 80, 80);
    lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_local_line_width(arc, LV_SPINNER_PART_BG, LV_STATE_DEFAULT,  4);
    lv_obj_set_style_local_line_color(arc, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
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
        lv_obj_set_style_local_text_font(label_arc,  LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
        lv_obj_set_style_local_text_color(label_arc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_obj_align(label_arc, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_local_line_color(arc, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED);
    }

    lv_obj_t *label = lv_label_create(parent, NULL);
    lv_label_set_text(label, success ? "Atualizado com sucesso" : "Erro ao atualizar");
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &ui_controller->styles.font_18_bold);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 35);

    /********************
     *  VALIDATE BUTTON
     ********************/
    lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(btn_validate, success ? smart_ring_ui_update_complete_handler 
                                              : smart_ring_ui_update_failed_handler);
    lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &ui_controller->styles.blue_button);
    lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
    lv_obj_align(btn_validate, NULL, LV_ALIGN_OUT_BOTTOM_MID, 00, -60);

    lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
    lv_label_set_text(label_btn, LV_SYMBOL_OK);
}
 


static void smart_ring_ui_update_option_firmware_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_update_state(STATE_34);
        smart_ring_ui_get_controller()->flags.flag.update_firmware      = true;
    }
}


static void smart_ring_ui_update_option_pin_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_get_controller()->flags.flag.update_pin           = true;
        smart_ring_ui_update_state(STATE_7);
    }
}


static void smart_ring_ui_update_option_cancel_handler(lv_obj_t  *button, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        smart_ring_ui_get_controller()->standby_controller.has_callback = true;
        smart_ring_ui_update_state(STATE_11);
    }
}


void update_create_option_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Atualizar", ui_controller);

  lv_obj_t *label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN,  &(ui_controller->styles.font_16_normal));
  lv_label_set_text(label, " ");

  static lv_style_t style_shadow;
  lv_style_init(&style_shadow);
  lv_style_set_shadow_width(&style_shadow,  LV_STATE_DEFAULT, 10);
  lv_style_set_shadow_spread(&style_shadow, LV_STATE_DEFAULT, 5);

  static lv_style_t style_firmware;
  lv_style_init(&style_firmware);
  lv_style_set_transition_time(&style_firmware,   LV_STATE_PRESSED, 400);
  lv_style_set_transition_time(&style_firmware,   LV_STATE_DEFAULT, 0);
  lv_style_set_transition_delay(&style_firmware,  LV_STATE_DEFAULT, 500);
  lv_style_set_outline_width(&style_firmware,     LV_STATE_DEFAULT, 0);
  lv_style_set_outline_width(&style_firmware,     LV_STATE_PRESSED, 20);
  lv_style_set_outline_opa(&style_firmware,       LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_style_set_outline_opa(&style_firmware,       LV_STATE_FOCUSED, LV_OPA_COVER);   /*Just to be sure, the theme might use it*/
  lv_style_set_outline_opa(&style_firmware,       LV_STATE_PRESSED, LV_OPA_TRANSP);
  lv_style_set_transition_prop_1(&style_firmware, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_OPA);
  lv_style_set_transition_prop_2(&style_firmware, LV_STATE_DEFAULT, LV_STYLE_OUTLINE_WIDTH);

  
  static lv_style_t style_button;
  lv_style_init(&style_button);
  lv_style_set_border_color(&style_button, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  /********************
   * PIN UPDATE BUTTON
   ********************/
  lv_obj_t *btn_pinUpdate = lv_btn_create(parent, NULL);
  lv_obj_add_style(btn_pinUpdate, LV_BTN_PART_MAIN, &style_button);
  lv_obj_align(btn_pinUpdate, NULL, LV_ALIGN_CENTER, 0, 50);
  lv_obj_add_style(btn_pinUpdate, LV_BTN_PART_MAIN, &style_firmware);
  lv_obj_set_event_cb(btn_pinUpdate, smart_ring_ui_update_option_pin_handler); 
  lv_obj_set_style_local_value_str(btn_pinUpdate, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Atualizar PIN");
 
  /*************************
   * FIRMWARE UPDATE BUTTON
   *************************/

 
  lv_obj_t *btn_firmwareUpdate = lv_btn_create(parent, NULL);
  lv_obj_add_style(btn_firmwareUpdate, LV_BTN_PART_MAIN, &style_button);
  lv_obj_align(btn_firmwareUpdate, NULL, LV_ALIGN_CENTER, 0, -30);
  lv_obj_add_style(btn_firmwareUpdate, LV_BTN_PART_MAIN, &style_firmware);
  lv_obj_set_event_cb(btn_firmwareUpdate, smart_ring_ui_update_option_firmware_handler);
  lv_obj_set_style_local_value_str(btn_firmwareUpdate, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Atualizar firmware");


  /******************
   *  CANCEL BUTTON
   ******************/
  lv_obj_t *img_btn_back = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(img_btn_back, smart_ring_ui_update_option_cancel_handler);
  lv_btn_set_fit(img_btn_back, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_all(img_btn_back,     LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 10);
  lv_obj_set_style_local_radius(img_btn_back,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 0);
  lv_obj_set_style_local_border_opa(img_btn_back,  LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_0);
  lv_obj_set_style_local_bg_opa(img_btn_back,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_100);
  lv_obj_set_style_local_bg_color(img_btn_back,    LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  lv_obj_set_style_local_outline_opa(img_btn_back, LV_BTN_PART_MAIN, LV_STATE_FOCUSED, LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_back,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_back, NULL, LV_ALIGN_IN_BOTTOM_MID, 110, -5);

  lv_obj_t *img_btn_back_img = lv_img_create(img_btn_back, NULL);
  lv_img_set_src(img_btn_back_img, &icon_logout);
}

/* END OF FILE */