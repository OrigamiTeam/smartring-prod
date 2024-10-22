/**
 * @file pin.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Responsible for the creation of the pin insertion screen
 * @version 2.0
 * @date 2022-06-23
 *
 * @copyright Copyright (c) 2023
 *
 * 
 

 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄        ▄     ▄▄▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░▌      ▐░▌   ▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ ▐░▌░▌     ▐░▌   ▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌       ▐░▌     ▐░▌     ▐░▌▐░▌    ▐░▌   ▐░▌          
▐░█▄▄▄▄▄▄▄█░▌     ▐░▌     ▐░▌ ▐░▌   ▐░▌   ▐░▌          
▐░░░░░░░░░░░▌     ▐░▌     ▐░▌  ▐░▌  ▐░▌   ▐░▌          
▐░█▀▀▀▀▀▀▀▀▀      ▐░▌     ▐░▌   ▐░▌ ▐░▌   ▐░▌          
▐░▌               ▐░▌     ▐░▌    ▐░▌▐░▌   ▐░▌          
▐░▌           ▄▄▄▄█░█▄▄▄▄ ▐░▌     ▐░▐░▌ ▄ ▐░█▄▄▄▄▄▄▄▄▄ 
▐░▌          ▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░▌▐░░░░░░░░░░░▌
 ▀            ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀  ▀  ▀▀▀▀▀▀▀▀▀▀▀ 
                                                       
  

 */

#include "pin.h"

// Tag for logging to the monitor
static const char *TAG = "UI_PIN";

static lv_obj_t   *inserted_pin;
static const char *map_keyboard[] = {"1", "2",  "3",  "\n", "4",
                                     "5", "6",  "\n", "7",  "8",
                                     "9", "\n", " ",  "0",  LV_SYMBOL_BACKSPACE,
                                     ""};

char *pin_get_inserted_pin() {
  char *pin = lv_textarea_get_text(inserted_pin);

#ifndef NDEBUG
  ESP_LOGI(TAG, "INSERTED PIN : %s", pin);
#endif

  return pin;
}

void pin_sign_in_handler(lv_obj_t *button, lv_event_t *event) {
  if (event == LV_EVENT_RELEASED && strlen(pin_get_inserted_pin()) == 4) {
      smart_ring_ui_get_controller()->standby_controller.has_callback = true;
      if(!smart_ring_ui_get_controller()->flags.flag.update_pin)
         smart_ring_ui_update_state(STATE_8);
      else
         smart_ring_ui_update_state(STATE_44);
      return;
  }
}

void pin_cancel_sign_in_handler(lv_obj_t *button, lv_event_t *event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_get_controller()->flags.flag.update_pin           = false;
    smart_ring_ui_update_state(STATE_5);
  }
}

void create_insert_pin_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  lv_obj_set_style_local_bg_color(parent, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  /************
   * Keyboard
   ************/
  static lv_style_t style_keyboard;
  lv_style_init(&style_keyboard);
  lv_style_set_bg_color(&style_keyboard, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_border_width(&style_keyboard, LV_STATE_DEFAULT, 0);
  lv_style_set_text_font(&style_keyboard, LV_STATE_DEFAULT, &lv_font_montserrat_24);

  static lv_style_t style_pin_keys;
  lv_style_init(&style_pin_keys);
  lv_style_set_bg_color(&style_pin_keys, LV_STATE_PRESSED,   LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_text_color(&style_pin_keys, LV_STATE_PRESSED, LV_COLOR_WHITE);

  lv_btnmatrix_ctrl_t keyboard_ctrl_map[12] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1 | LV_BTNMATRIX_CTRL_HIDDEN, 1, 1};

  lv_obj_t *keyboard = lv_keyboard_create(parent, NULL);
  lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_NUM, map_keyboard);
  lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUM);
  lv_btnmatrix_set_ctrl_map(keyboard, keyboard_ctrl_map);

  lv_btnmatrix_set_one_check(keyboard, true);
  lv_obj_add_style(keyboard, LV_KEYBOARD_PART_BG, &style_keyboard);
  lv_obj_add_style(keyboard, LV_KEYBOARD_PART_BTN, &style_keyboard);
  lv_obj_add_style(keyboard, LV_KEYBOARD_PART_BTN, &style_pin_keys);
  // lv_obj_add_style(keyboard, LV_KEYBOARD_PART_BTN,
  //                  &(ui_controller->styles.font_24_normal));
  lv_obj_set_size(keyboard, 192, 200);
  lv_obj_align(keyboard, NULL, LV_ALIGN_CENTER, -55, 0);
  lv_keyboard_set_cursor_manage(keyboard, true);
  lv_btnmatrix_set_one_check(keyboard, true);
  lv_btnmatrix_set_btn_ctrl_all(keyboard, LV_BTNMATRIX_CTRL_NO_REPEAT);

  /*********************
   * Keyboard Text Area
   *********************/
  static lv_style_t style_font_blue;
  lv_style_init(&style_font_blue);
  lv_style_set_text_color(&style_font_blue, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  static lv_style_t style_inserted_pin;
  lv_style_init(&style_inserted_pin);
  lv_style_set_border_width(&style_inserted_pin, LV_STATE_DEFAULT, 0);
  lv_style_set_text_letter_space(&style_inserted_pin, LV_STATE_DEFAULT, 9);
  lv_style_set_bg_opa(&style_inserted_pin, LV_STATE_DEFAULT, LV_OPA_TRANSP);

  static lv_style_t style_ta_cursor;
  lv_style_init(&style_ta_cursor);
  lv_style_set_border_width(&style_ta_cursor, LV_STATE_DEFAULT, 0);

#ifndef NDEBUG
  printf("Creating textarea\n");
#endif
  inserted_pin = lv_textarea_create(parent, NULL);
#ifndef NDEBUG
  printf("Adding textarea styles background\n");
#endif
  lv_obj_add_style(inserted_pin, LV_TEXTAREA_PART_BG, &style_inserted_pin);
#ifndef NDEBUG
  printf("Adding textarea font\n");
#endif
  lv_obj_add_style(inserted_pin, LV_TEXTAREA_PART_BG, &style_font_blue);
  lv_obj_add_style(inserted_pin, LV_TEXTAREA_PART_BG, 
          smart_ring_ui_get_controller()->flags.flag.update_pin ? &(ui_controller->styles.font_18_normal) : &(ui_controller->styles.font_26_normal));
#ifndef NDEBUG
  printf("Adding textarea cursor\n");
#endif
  lv_obj_add_style(inserted_pin, LV_TEXTAREA_PART_CURSOR, &style_ta_cursor);
#ifndef NDEBUG
  printf("Adding properties\n");
#endif
  lv_obj_set_width(inserted_pin, 92);
  lv_obj_align(inserted_pin, keyboard, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_textarea_set_one_line(inserted_pin, true);
  lv_textarea_set_text(inserted_pin, "");
  lv_textarea_set_max_length(inserted_pin, 4);
  lv_textarea_set_pwd_mode(inserted_pin, smart_ring_ui_get_controller()->flags.flag.update_pin ? false : true);
  lv_textarea_set_pwd_show_time(inserted_pin, 0);
  lv_textarea_set_scrollbar_mode(inserted_pin, LV_SCROLLBAR_MODE_OFF);
  lv_textarea_set_cursor_click_pos(inserted_pin, false);

#ifndef NDEBUG
  printf("Assigning textarea to keyboard\n");
#endif
  lv_keyboard_set_textarea(keyboard, inserted_pin);

  /* Labels */
  lv_obj_t *label_pin = lv_label_create(parent, NULL);
  lv_obj_add_style(label_pin, LV_LABEL_PART_MAIN, &style_font_blue);
  lv_obj_add_style(label_pin, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_text(label_pin, smart_ring_ui_get_controller()->flags.flag.update_pin ? "Alterar PIN" : "Inserir PIN");
  lv_obj_align(label_pin, inserted_pin, LV_ALIGN_OUT_TOP_MID, 0, 0);

  static lv_style_t style_underscore;
  lv_style_init(&style_underscore);
  lv_style_set_text_letter_space(&style_underscore, LV_STATE_DEFAULT, 5);

  label_pin = lv_label_create(parent, NULL);
  lv_obj_set_width(label_pin, 92);
  lv_obj_set_style_local_text_letter_space(label_pin, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 7);
  lv_obj_add_style(label_pin, LV_LABEL_PART_MAIN, &style_inserted_pin);
  lv_obj_add_style(label_pin, LV_LABEL_PART_MAIN, &style_font_blue);
  lv_obj_add_style(label_pin, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_26_normal));
  lv_obj_add_style(label_pin, LV_LABEL_PART_MAIN, &style_underscore);
  lv_label_set_text(label_pin, "____");
  lv_obj_align(label_pin, inserted_pin, LV_ALIGN_CENTER, -1, 5);

  /********************
   *  VALIDATE BUTTON
   ********************/
  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, pin_sign_in_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &ui_controller->styles.blue_button);
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, inserted_pin, LV_ALIGN_OUT_BOTTOM_MID, 0, 22);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, smart_ring_ui_get_controller()->flags.flag.update_pin ? "Atualizar" : "Validar");
    

  /******************
   *  CANCEL BUTTON
   ******************/
  static lv_style_t style_btn_red;
  lv_style_init(&style_btn_red);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);

  lv_obj_t *btn_cancel = lv_btn_create(parent, btn_validate);
  lv_obj_set_event_cb(btn_cancel, pin_cancel_sign_in_handler);
  lv_obj_add_style(btn_cancel, LV_BTN_PART_MAIN, &style_btn_red);
  lv_obj_align(btn_cancel, btn_validate, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

  label_btn = lv_label_create(btn_cancel, NULL);
  lv_label_set_text(label_btn, "Cancelar");
}