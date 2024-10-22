/**
 * @file calibration.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Responsible for the calibration screns
 * @version 2.0
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2023
 *
 * 
            _ _ _               _   _                   
   ___ __ _| (_) |__  _ __ __ _| |_(_) ___  _ __    ___ 
  / __/ _` | | | '_ \| '__/ _` | __| |/ _ \| '_ \  / __|
 | (_| (_| | | | |_) | | | (_| | |_| | (_) | | | || (__ 
  \___\__,_|_|_|_.__/|_|  \__,_|\__|_|\___/|_| |_(_)___|
                                                        

 */

#include "calibration.h"


LV_IMG_DECLARE(spinner_check)

static void smart_ring_calibration_no_bottle_check_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_get_controller()->flags.flag.start_calibration = true;
    smart_ring_ui_update_state(STATE_30);
  }
}

static void smart_ring_calibration_full_bottle_check_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_30);
    smart_ring_ui_get_controller()->flags.flag.empty_calibration = true;
  }
}

static void smart_ring_calibration_cancel_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
     // esp_restart();
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_28);
    smart_ring_ui_get_controller()->flags.flag.cancel_calibration = true;
  }
}

void calibration_create_no_bottle_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  create_header(parent, "Calibrar", ui_controller);

#ifndef NDEBUG
  printf("CREATE HEADER\n");
#endif
  static lv_style_t style_header;
  lv_style_init(&style_header);
  lv_style_set_text_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_GRAY);

  lv_obj_t *header = lv_label_create(parent, NULL);
  lv_obj_add_style(header, LV_LABEL_PART_MAIN, &style_header);
  lv_obj_add_style(header, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_label_set_long_mode(header, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(header, 280);
  lv_label_set_align(header, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(header, "Retire o garrafão da máquina e pressione confirmar.");
  lv_obj_align(header, NULL, LV_ALIGN_CENTER, 0, -50);

#ifndef NDEBUG
  printf("CREATE SUB HEADER\n");
#endif
  static lv_style_t style_sub_label;
  lv_style_init(&style_sub_label);
  lv_style_set_text_color(&style_sub_label, LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_obj_t *sub_label = lv_label_create(parent, NULL);
  lv_obj_add_style(sub_label, LV_LABEL_PART_MAIN, &style_sub_label);
  lv_obj_add_style(sub_label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_long_mode(sub_label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(sub_label, 280);
  lv_label_set_align(sub_label, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(sub_label, "Certifique-se que o tanque interno da máquina se encontra cheio.");
  lv_obj_align(sub_label, NULL, LV_ALIGN_CENTER, 0, 20);

  /********************
   *  VALIDATE BUTTON *
   ********************/
#ifndef NDEBUG
  printf("CREATE VALIDATE BUTTON\n");
#endif
  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, smart_ring_calibration_no_bottle_check_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, NULL, LV_ALIGN_IN_BOTTOM_MID, -60, -20);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_OK);

  /******************
   *  CANCEL BUTTON *
   ******************/
#ifndef NDEBUG
  printf("CREATE CANCEL BUTTON\n");
#endif
  static lv_style_t style_btn_red;
  lv_style_init(&style_btn_red);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);

  lv_obj_t *btn_cancel = lv_btn_create(parent, btn_validate);
  lv_obj_set_event_cb(btn_cancel, smart_ring_calibration_cancel_handler);
  lv_obj_add_style(btn_cancel, LV_BTN_PART_MAIN, &style_btn_red);
  lv_obj_align(btn_cancel, btn_validate, LV_ALIGN_CENTER, 120, 0);

  label_btn = lv_label_create(btn_cancel, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_CLOSE);
}

void calibration_create_full_bottle_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  create_header(parent, "Calibrar", ui_controller);

#ifndef NDEBUG
  printf("CREATE HEADER\n");
#endif
  static lv_style_t style_header;
  lv_style_init(&style_header);
  lv_style_set_text_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_GRAY);

  lv_obj_t *header = lv_label_create(parent, NULL);
  lv_obj_add_style(header, LV_LABEL_PART_MAIN, &style_header);
  lv_obj_add_style(header, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_label_set_long_mode(header, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(header, 280);
  lv_label_set_align(header, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(header, "Insira o garrafão cheio e pressione confirmar.");
  lv_obj_align(header, NULL, LV_ALIGN_CENTER, 0, -50);

#ifndef NDEBUG
  printf("CREATE SUB HEADER\n");
#endif
  static lv_style_t style_sub_label;
  lv_style_init(&style_sub_label);
  lv_style_set_text_color(&style_sub_label, LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_obj_t *sub_label = lv_label_create(parent, NULL);
  lv_obj_add_style(sub_label, LV_LABEL_PART_MAIN, &style_sub_label);
  lv_obj_add_style(sub_label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_long_mode(sub_label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(sub_label, 280);
  lv_label_set_align(sub_label, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(sub_label,"Certifique-se que o tanque interno da máquina se encontra cheio.");
  lv_obj_align(sub_label, NULL, LV_ALIGN_CENTER, 0, 20);

  /********************
   *  VALIDATE BUTTON *
   ********************/
#ifndef NDEBUG
  printf("CREATE VALIDATE BUTTON\n");
#endif
  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, smart_ring_calibration_full_bottle_check_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN,&(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, NULL, LV_ALIGN_IN_BOTTOM_MID, -60, -20);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_OK);

  /******************
   *  CANCEL BUTTON *
   ******************/
#ifndef NDEBUG
  printf("CREATE CANCEL BUTTON\n");
#endif
  static lv_style_t style_btn_red;
  lv_style_init(&style_btn_red);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);

  lv_obj_t *btn_cancel = lv_btn_create(parent, btn_validate);
  lv_obj_set_event_cb(btn_cancel, smart_ring_calibration_cancel_handler);
  lv_obj_add_style(btn_cancel, LV_BTN_PART_MAIN, &style_btn_red);
  lv_obj_align(btn_cancel, btn_validate, LV_ALIGN_CENTER, 120, 0);

  label_btn = lv_label_create(btn_cancel, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_CLOSE);
}

struct smart_ring_ui_calibration_progress_container_t {
  lv_obj_t *label_title;
  lv_obj_t *label_subtitle;
  lv_obj_t *spinner;
  lv_obj_t *img_spinner;
} container_progress;

void smart_ring_ui_calibration_update_progress_screen(bool finished) {

  if (finished) {
    lv_spinner_set_arc_length(container_progress.spinner, 359);
    lv_obj_set_hidden(container_progress.img_spinner, false);
    lv_label_set_text(container_progress.label_title, "Estabilizado");
    lv_label_set_text(container_progress.label_subtitle, "Valor armazenado com sucesso");
  } 
  else {
    lv_spinner_set_arc_length(container_progress.spinner, 60);
    lv_obj_set_hidden(container_progress.img_spinner, true);
    lv_label_set_text(container_progress.label_title, "A estabilizar");
    lv_label_set_text(container_progress.label_subtitle, "Por favor aguarde");
  }
}

void calibration_create_progress_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Calibrar", ui_controller);

  printf("CREATING BACKGROUND\n");
  lv_obj_t *background = lv_obj_create(parent, NULL);
  lv_obj_set_size(background, 320, 210);
  lv_obj_align(background, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);
  lv_obj_set_style_local_bg_color(background, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_obj_set_style_local_bg_opa(background,   LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_30);
  lv_obj_set_style_local_radius(background,   LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

  printf("CREATING MODAL\n");
  lv_obj_t *modal = lv_obj_create(parent, NULL);
  lv_obj_set_size(modal, 280, 150);
  lv_obj_align(modal, NULL, LV_ALIGN_CENTER, 0, -10);
  lv_obj_set_style_local_bg_color(modal, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

  printf("CREATING SPINNER\n");
  container_progress.spinner = lv_spinner_create(modal, NULL);
  lv_obj_set_size(container_progress.spinner, 65, 65);
  lv_obj_align(container_progress.spinner, NULL, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_local_line_width(container_progress.spinner, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_line_color(container_progress.spinner, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
  lv_obj_set_style_local_line_width(container_progress.spinner, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_line_color(container_progress.spinner, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_spinner_set_arc_length(container_progress.spinner, 60);

  container_progress.img_spinner = lv_img_create(container_progress.spinner, NULL);
  lv_img_set_src(container_progress.img_spinner, &spinner_check);
  lv_img_set_zoom(container_progress.img_spinner, 160);
  lv_obj_align(container_progress.img_spinner, container_progress.spinner, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_hidden(container_progress.img_spinner, true);

  printf("CREATING HEADER\n");
  static lv_style_t style_header;
  lv_style_init(&style_header);
  lv_style_set_text_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_GRAY);

  container_progress.label_title = lv_label_create(modal, NULL);
  lv_obj_add_style(container_progress.label_title, LV_LABEL_PART_MAIN, &style_header);
  lv_obj_add_style(container_progress.label_title, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_label_set_long_mode(container_progress.label_title, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(container_progress.label_title, 280);
  lv_label_set_align(container_progress.label_title, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(container_progress.label_title, "A estabilizar");
  lv_obj_align(container_progress.label_title, NULL, LV_ALIGN_CENTER, 0, 25);

  printf("CREATING SUBTITLE\n");
  static lv_style_t style_sub_label;
  lv_style_init(&style_sub_label);
  lv_style_set_text_color(&style_sub_label, LV_STATE_DEFAULT, LV_COLOR_GRAY);

  container_progress.label_subtitle = lv_label_create(parent, NULL);
  lv_obj_add_style(container_progress.label_subtitle, LV_LABEL_PART_MAIN, &style_sub_label);
  lv_obj_add_style(container_progress.label_subtitle, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_long_mode(container_progress.label_subtitle, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(container_progress.label_subtitle, 280);
  lv_label_set_align(container_progress.label_subtitle, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(container_progress.label_subtitle, "Por favor aguarde");
  lv_obj_align(container_progress.label_subtitle, NULL, LV_ALIGN_CENTER, 0, 40);

  /******************
   *  CANCEL BUTTON *
   ******************/
  printf("CREATING CANCEL BUTTON\n");
  static lv_style_t style_btn_red;
  lv_style_init(&style_btn_red);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);

  lv_obj_t *btn_cancel = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_cancel, smart_ring_calibration_cancel_handler);
  lv_obj_add_style(btn_cancel, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_add_style(btn_cancel, LV_BTN_PART_MAIN, &style_btn_red);
  lv_obj_set_size(btn_cancel, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_cancel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

  lv_obj_t *label_btn = lv_label_create(btn_cancel, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_CLOSE);
}

void calibration_create_finished_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  create_header(parent, "Calibrar", ui_controller);

  lv_obj_t *arc = lv_spinner_create(parent, NULL);
  lv_obj_set_size(arc, 65, 65);
  lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, -40);
  lv_obj_set_style_local_line_width(arc, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_line_color(arc, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
  lv_obj_set_style_local_line_width(arc, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_line_color(arc, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_spinner_set_arc_length(arc, 359);

  lv_obj_t *arc_img = lv_img_create(arc, NULL);
  lv_img_set_src(arc_img, &spinner_check);
  lv_img_set_zoom(arc_img, 160);
  lv_obj_align(arc_img, arc, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_hidden(arc_img, false);

  static lv_style_t style_header;
  lv_style_init(&style_header);
  lv_style_set_text_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_GRAY);

  lv_obj_t *calibrating_header = lv_label_create(parent, NULL);
  lv_obj_add_style(calibrating_header, LV_LABEL_PART_MAIN, &style_header);
  lv_obj_add_style(calibrating_header, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_label_set_long_mode(calibrating_header, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(calibrating_header, 250);
  lv_label_set_align(calibrating_header, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(calibrating_header, "Calibração realizada com sucesso");
  lv_obj_align(calibrating_header, NULL, LV_ALIGN_CENTER, 0, 25);

  /********************
   *  VALIDATE BUTTON *
   ********************/
  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, smart_ring_calibration_cancel_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_OK);


}

