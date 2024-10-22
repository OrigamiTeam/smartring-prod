/**
 * @file boot.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the creation of the boot
 * screens
 *
 *
 * @version 2.1.1
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

//====================//
//      INCLUDES      //
//====================//
#include "boot.h"
#include "libs.h"

// Tag for logging to the monitor
static const char *TAG = "UI_BOOT";

//====================//
//   LVGL DECLARES    //
//====================//
LV_IMG_DECLARE(icon_smartring)
LV_IMG_DECLARE(origami_logo)

static lv_obj_t *warning_label;
static lv_obj_t *btn_reset_wifi;
static lv_obj_t *btn_change;

static void boot_menu_select_communication_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
      smart_ring_ui_get_controller()->standby_controller.has_callback = true;
      char communication = lv_obj_get_user_data(button);

#ifndef NDEBUG
    ESP_LOGI(TAG, "COM SELECTED : %c", communication);
#endif

    switch (communication) {
      case 'w': smart_ring_ui_set_connection_type('w');
                smart_ring_ui_get_controller()->flags.flag.select_communication = true;
                break;
      default:  break;
    }
  }
}

static void boot_change_communication_type(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
      smart_ring_ui_get_controller()->standby_controller.has_callback = true;
#ifndef NDEBUG
    ESP_LOGI(TAG, "Change Communication type screen");
#endif
      esp_timer_stop(smart_ring_ui_get_controller()->timer);
      smart_ring_ui_update_state(STATE_1);
  }
}

static void boot_reset_wifi(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
      smart_ring_ui_get_controller()->standby_controller.has_callback = true;
#ifndef NDEBUG
      ESP_LOGI(TAG, "Reset WiFi interface");
#endif
      esp_timer_stop(smart_ring_ui_get_controller()->timer);
      modal_create_template(lv_scr_act(), smart_ring_ui_get_controller(), "Reset Wi-Fi", "Por favor aguarde", true);
      smart_ring_ui_get_controller()->flags.flag.reset_wifi = true;
  }
}

void boot_update_warning_label(char *text) {
  lv_label_set_text(warning_label, text);
}

void boot_update_connection_screen() {
  lv_obj_set_hidden(btn_reset_wifi, true);
  lv_obj_set_hidden(btn_change, true);

  lv_obj_align(warning_label, NULL, LV_ALIGN_CENTER, 0, 40);
  lv_label_set_text(warning_label, "A conectar-se à rede");
}

static void
create_boot_header(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

#ifndef NDEBUG
  printf("Styling header\n");
#endif
  static lv_style_t style_boot_header;
  lv_style_init(&style_boot_header);
  lv_style_set_bg_color(&style_boot_header,     LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x85, 0xB0));
  lv_style_set_text_color(&style_boot_header,   LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_text_font(&style_boot_header,    LV_STATE_DEFAULT, &(ui_controller->styles.font_18_normal));
  lv_style_set_radius(&style_boot_header,       LV_STATE_DEFAULT, 0);
  lv_style_set_border_width(&style_boot_header, LV_STATE_DEFAULT, 2);
  lv_style_set_border_side(&style_boot_header,  LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
  lv_style_set_border_color(&style_boot_header, LV_STATE_DEFAULT, LV_COLOR_WHITE);

#ifndef NDEBUG
  printf("Creating header\n");
#endif
  lv_obj_t *header = lv_cont_create(parent, NULL);
  lv_obj_add_style(header, LV_CONT_PART_MAIN, &style_boot_header);
  lv_obj_set_size(header, 320, 30);
  lv_obj_align(header, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);

#ifndef NDEBUG
  printf("Creating label\n");
#endif
  lv_obj_t *label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_normal));
  lv_label_set_text(label, "Tipo de Comunicação");
  lv_obj_align(label, header, LV_ALIGN_CENTER, 0, 0);

#ifndef NDEBUG
  printf("Finished header\n");
#endif
}

void create_booting_up_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
#ifndef NDEBUG
  printf("CREATING BOOTING UP SCREEN\nStarting styles\n");
#endif

  /* Local Styling */
  static lv_style_t style_boot_screen;
  lv_style_init(&style_boot_screen);
  lv_style_set_bg_color(&style_boot_screen,   LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x85, 0xB0));
  lv_style_set_text_color(&style_boot_screen, LV_STATE_DEFAULT, LV_COLOR_WHITE);

  static lv_style_t style_boot_text;
  lv_style_init(&style_boot_text);
  lv_style_set_text_color(&style_boot_text, LV_STATE_DEFAULT, LV_COLOR_WHITE);

#ifndef NDEBUG
  printf("Styles created\nCreating screen\n");
#endif

#ifndef NDEBUG
  printf("Adding styles\n");
#endif
  /* Adding styles*/
  lv_obj_add_style(parent, LV_OBJ_PART_MAIN, &style_boot_screen);

#ifndef NDEBUG
  printf("Creating screen content\n");
#endif
  /* Creating content */
  lv_obj_t *img_smartring = lv_img_create(parent, NULL);
  lv_img_set_src(img_smartring, &icon_smartring);
  lv_obj_align(img_smartring, NULL, LV_ALIGN_IN_TOP_LEFT, 41, 36);

  lv_obj_t *img_logo = lv_img_create(parent, NULL);
  lv_img_set_src(img_logo, &origami_logo);
  lv_obj_align(img_logo, NULL, LV_ALIGN_IN_TOP_LEFT, 51, 104);

  lv_obj_t *label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_boot_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_12_normal));
  lv_label_set_text(label, "© 2024 Origami\na Quantum Leap division");
  lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 6, -4);

  label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_boot_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_12_normal));

  char aux_firmware_version[32];
  sprintf(aux_firmware_version, "firmware %s", ui_controller->firmware_version);
  lv_label_set_text(label, aux_firmware_version);
  lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -6, -4);

#ifndef NDEBUG
  printf("FINISHED CREATING BOOTING UP SCREEN\n");
#endif
}

void create_selected_communication(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

#ifndef NDEBUG
  printf("CREATING BOOTING UP SCREEN\nStarting styles\n");
#endif
  lv_obj_add_style(parent, LV_OBJ_PART_MAIN, &(ui_controller->styles.boot_communications));
  create_boot_header(parent, ui_controller);

  lv_obj_t *label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_18_normal));
  lv_label_set_text(label, "Comunicação selecionada:");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -54);

  char com_type_string[16];
  strcpy(com_type_string, "WIFI");
  switch (ui_controller->connection_type) {
      case 'w': strcpy(com_type_string, "WiFi"); break;
      case 'g': strcpy(com_type_string, "GSM");  break;
      case 'l': strcpy(com_type_string, "Lora"); break;
      default:  strcpy(com_type_string, "ERRO"); break;
  }

  label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_22_normal));
  lv_label_set_text(label, com_type_string);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -20);

  warning_label = lv_label_create(parent, NULL);
  lv_obj_add_style(warning_label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_18_normal));
  lv_label_set_text(warning_label, "Aguarde 5 segundos para avançar");
  lv_obj_align(warning_label, NULL, LV_ALIGN_CENTER, 0, 15);
  lv_obj_set_auto_realign(warning_label, true);

  btn_change = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_change, boot_change_communication_type);
  lv_obj_add_style(btn_change, LV_BTN_PART_MAIN, &(ui_controller->styles.boot_button));
  lv_obj_set_size(btn_change, SR_MEDIUM_BTN_SIZE);
  lv_obj_align(btn_change, NULL, LV_ALIGN_IN_BOTTOM_MID, -76, -34);

  label = lv_label_create(btn_change, NULL);
  lv_label_set_text(label, "Mudar");

  btn_reset_wifi = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_reset_wifi, boot_reset_wifi);
  lv_obj_add_style(btn_reset_wifi, LV_BTN_PART_MAIN, &(ui_controller->styles.boot_button));
  lv_obj_set_size(btn_reset_wifi, SR_MEDIUM_BTN_SIZE);
  lv_obj_align(btn_reset_wifi, NULL, LV_ALIGN_IN_BOTTOM_MID, 76, -34);

  label = lv_label_create(btn_reset_wifi, NULL);
  lv_label_set_text(label, "Reset Wi-Fi");

  lv_obj_t *label_boot_message = lv_label_create(parent, NULL);
  lv_obj_add_style(label_boot_message, LV_STATE_DEFAULT, &(ui_controller->styles.font_16_normal));
  lv_label_set_text(label_boot_message, "");
  lv_label_set_align(label_boot_message, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label_boot_message, NULL, LV_ALIGN_IN_TOP_MID, 0, 150);

#ifndef NDEBUG
  printf("FINISHED CREATING BOOTING UP SCREEN\n");
#endif
}

void create_communication_selection(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  lv_obj_add_style(parent, LV_OBJ_PART_MAIN, &(ui_controller->styles.boot_communications));

  create_boot_header(parent, ui_controller);

  lv_obj_t *btn_wifi = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_wifi, boot_menu_select_communication_handler);
  lv_obj_add_style(btn_wifi, LV_BTN_PART_MAIN, &(ui_controller->styles.boot_button));
  lv_obj_set_size(btn_wifi, 250, 62);
  lv_obj_align(btn_wifi, NULL, LV_ALIGN_IN_TOP_LEFT, 30, 58);

  char communication_type = 'w';
  lv_obj_set_user_data(btn_wifi, communication_type);

  lv_obj_t *label = lv_label_create(btn_wifi, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_18_normal));
  lv_label_set_text(label, "WIFI");

  lv_obj_t *btn_gsm = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_gsm, boot_menu_select_communication_handler);
  lv_obj_add_style(btn_gsm, LV_BTN_PART_MAIN, &(ui_controller->styles.boot_button));

  lv_obj_set_style_local_bg_color(btn_gsm,   LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x30, 0x30, 0x30));
  lv_obj_set_style_local_text_color(btn_gsm, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
  lv_obj_set_click(btn_gsm, false);
  lv_obj_set_size(btn_gsm, 120, 62);
  lv_obj_align(btn_gsm, NULL, LV_ALIGN_IN_TOP_LEFT, 30, 134);

  communication_type = 'g';
  lv_obj_set_user_data(btn_gsm, communication_type);

  label = lv_label_create(btn_gsm, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_18_normal));
  lv_label_set_text(label, "GSM");

  lv_obj_t *btn_rf = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_rf, boot_menu_select_communication_handler);
  lv_obj_add_style(btn_rf, LV_BTN_PART_MAIN, &(ui_controller->styles.boot_button));
  lv_obj_set_size(btn_rf, 120, 62);
  lv_obj_align(btn_rf, NULL, LV_ALIGN_IN_TOP_LEFT, 158, 134);

  lv_obj_set_style_local_bg_color(btn_rf,   LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x30, 0x30, 0x30));
  lv_obj_set_style_local_text_color(btn_rf, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
  lv_obj_set_click(btn_rf, false);

  communication_type = 'l';
  lv_obj_set_user_data(btn_rf, communication_type);

  label = lv_label_create(btn_rf, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_18_normal));
  lv_label_set_text(label, "RF");
}

/* END OF FILE*/