/**
 * @file smart_ring_ui.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief The entry point for the display functions, responsible for controlling
 * the current state and apply the necessary changes on the screen
 * @version 2.0
 * @date 2022-06-21
 *
 * @copyright Copyright (c) 2023
 *
 */

//====================//
//      INCLUDES      //
//====================//

#include "smart_ring_ui.h"

// Tag for logging to the monitor
static const char *TAG = "UI_CONTROLLER";

//====================//
//   LVGL DECLARES    //
//====================//
LV_FONT_DECLARE(sr_font_montserrat_12)
LV_FONT_DECLARE(sr_font_montserrat_14)
LV_FONT_DECLARE(sr_font_montserrat_16)
LV_FONT_DECLARE(sr_font_montserrat_16_bold)
LV_FONT_DECLARE(sr_font_montserrat_18)
LV_FONT_DECLARE(sr_font_montserrat_18_bold)
LV_FONT_DECLARE(sr_font_montserrat_22)
LV_FONT_DECLARE(sr_font_montserrat_24)
LV_FONT_DECLARE(sr_font_montserrat_26)

// Controller for the ui system
static struct smart_ring_ui_controller_t smart_ring_ui_controller = {
    .screen           = NULL,
    .menu             = MENU_ID_BOOT,
    .state            = STATE_0,
    .old_state        = STATE_0,
    .flags.has_flags  = 0x00,
    .timer_type       = CLOSE_MODAL,
    .timer            = NULL,
    .firmware_version = "0.0.0",
    .device_name      = "Smartring",
    .mac_address      = "",
    .stock            = 0,
    .support_type     = 1,
    .order_mode       = 'a',
    .ssid             = "",
    .connection_type  = 'w',
    .no_deposit       = 0,
    .full_deposit     = 0,
    .current_deposit  = 0,
    .is_stable        = false,
    .rssi_level       = 0,
    .is_admin         = false,
    .is_master        = false,
    .order.changing_bottles = true,
};

esp_err_t smart_ring_ui_init_system(void) {

  /* Blank page style */
  lv_style_init(&(smart_ring_ui_controller.styles.blank_page));
  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.blank_page), LV_STATE_DEFAULT, LV_COLOR_WHITE);

  /* Blue buttons style */
  lv_style_init(&(smart_ring_ui_controller.styles.blue_button));
  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.blue_button),       LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_radius(&(smart_ring_ui_controller.styles.blue_button),         LV_STATE_DEFAULT, SR_DEFAULT_BTN_RADIUS);
  lv_style_set_border_width(&(smart_ring_ui_controller.styles.blue_button),   LV_STATE_DEFAULT, 0);
  lv_style_set_shadow_width(&(smart_ring_ui_controller.styles.blue_button),   LV_STATE_DEFAULT, 10);
  lv_style_set_shadow_ofs_y(&(smart_ring_ui_controller.styles.blue_button),   LV_STATE_DEFAULT, 5);
  lv_style_set_shadow_color(&(smart_ring_ui_controller.styles.blue_button),   LV_STATE_DEFAULT, LV_COLOR_MAKE(0x97, 0x97, 0x97));
  lv_style_set_text_color(&(smart_ring_ui_controller.styles.blue_button),     LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_text_sel_color(&(smart_ring_ui_controller.styles.blue_button), LV_STATE_DEFAULT, LV_COLOR_WHITE);
  // Pressed
  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.blue_button),       LV_STATE_PRESSED, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_radius(&(smart_ring_ui_controller.styles.blue_button),         LV_STATE_PRESSED, SR_DEFAULT_BTN_RADIUS);
  lv_style_set_bg_opa(&(smart_ring_ui_controller.styles.blue_button),         LV_STATE_PRESSED, LV_OPA_70);
  lv_style_set_border_width(&(smart_ring_ui_controller.styles.blue_button),   LV_STATE_PRESSED, 0);
  lv_style_set_text_color(&(smart_ring_ui_controller.styles.blue_button),     LV_STATE_PRESSED, LV_COLOR_WHITE);
  lv_style_set_text_sel_color(&(smart_ring_ui_controller.styles.blue_button), LV_STATE_PRESSED, LV_COLOR_WHITE);

  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.blue_button),      LV_STATE_DISABLED, LV_COLOR_MAKE(0xEB, 0xED, 0xF0));
  lv_style_set_border_width(&(smart_ring_ui_controller.styles.blue_button),  LV_STATE_DISABLED, 0);
  lv_style_set_outline_width(&(smart_ring_ui_controller.styles.blue_button), LV_STATE_DISABLED, 0);
  //*/

  /* White buttons style */
  lv_style_init(&(smart_ring_ui_controller.styles.white_button));
  lv_style_set_radius(&(smart_ring_ui_controller.styles.white_button),         LV_STATE_DEFAULT, SR_DEFAULT_BTN_RADIUS);
  lv_style_set_border_width(&(smart_ring_ui_controller.styles.white_button),   LV_STATE_DEFAULT, 1);
  lv_style_set_border_color(&(smart_ring_ui_controller.styles.white_button),   LV_STATE_DEFAULT, LV_COLOR_MAKE(0xEB, 0xED, 0xF0));
  lv_style_set_shadow_width(&(smart_ring_ui_controller.styles.white_button),   LV_STATE_DEFAULT, 10);
  lv_style_set_shadow_ofs_y(&(smart_ring_ui_controller.styles.white_button),   LV_STATE_DEFAULT, 5);
  lv_style_set_shadow_color(&(smart_ring_ui_controller.styles.white_button),   LV_STATE_DEFAULT, LV_COLOR_MAKE(0x97, 0x97, 0x97));
  lv_style_set_text_color(&(smart_ring_ui_controller.styles.white_button),     LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_text_sel_color(&(smart_ring_ui_controller.styles.white_button), LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  // Pressed
  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.white_button),       LV_STATE_PRESSED, LV_COLOR_WHITE);
  lv_style_set_radius(&(smart_ring_ui_controller.styles.white_button),         LV_STATE_PRESSED, SR_DEFAULT_BTN_RADIUS);
  lv_style_set_bg_grad_color(&(smart_ring_ui_controller.styles.white_button),  LV_STATE_PRESSED, LV_COLOR_MAKE(0x97, 0x97, 0x97));
  lv_style_set_bg_opa(&(smart_ring_ui_controller.styles.white_button),         LV_STATE_PRESSED, LV_OPA_100);
  lv_style_set_border_width(&(smart_ring_ui_controller.styles.white_button),   LV_STATE_PRESSED, 1);
  lv_style_set_border_color(&(smart_ring_ui_controller.styles.white_button),   LV_STATE_PRESSED, LV_COLOR_MAKE(0xEB, 0xED, 0xF0));
  lv_style_set_text_color(&(smart_ring_ui_controller.styles.white_button),     LV_STATE_PRESSED, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_text_sel_color(&(smart_ring_ui_controller.styles.white_button), LV_STATE_PRESSED, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  /* Boot buttons */
  lv_style_init(&(smart_ring_ui_controller.styles.boot_button));
  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.boot_button),       LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x80, 0x80));
  lv_style_set_text_color(&(smart_ring_ui_controller.styles.boot_button),     LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_text_sel_color(&(smart_ring_ui_controller.styles.boot_button), LV_STATE_PRESSED, LV_COLOR_WHITE);
  lv_style_set_radius(&(smart_ring_ui_controller.styles.boot_button),         LV_STATE_DEFAULT, SR_DEFAULT_BTN_RADIUS);
  lv_style_set_border_width(&(smart_ring_ui_controller.styles.boot_button),   LV_STATE_DEFAULT, 0);
  lv_style_set_shadow_width(&(smart_ring_ui_controller.styles.boot_button),   LV_STATE_DEFAULT, 10);
  lv_style_set_shadow_ofs_y(&(smart_ring_ui_controller.styles.boot_button),   LV_STATE_DEFAULT, 5);

  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.boot_button),       LV_STATE_PRESSED, LV_COLOR_WHITE);
  lv_style_set_text_color(&(smart_ring_ui_controller.styles.boot_button),     LV_STATE_PRESSED, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_text_sel_color(&(smart_ring_ui_controller.styles.boot_button), LV_STATE_PRESSED, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_radius(&(smart_ring_ui_controller.styles.boot_button),         LV_STATE_PRESSED, SR_DEFAULT_BTN_RADIUS);
  // lv_style_set_bg_opa(&(smart_ring_ui_controller.styles.boot_button),
  // LV_STATE_PRESSED, LV_OPA_70);
  lv_style_set_outline_color(&(smart_ring_ui_controller.styles.boot_button), LV_STATE_PRESSED, LV_COLOR_MAKE(0x80, 0x80, 0x80));
  lv_style_set_border_width(&(smart_ring_ui_controller.styles.boot_button),  LV_STATE_PRESSED, 0);
  lv_style_set_shadow_width(&(smart_ring_ui_controller.styles.boot_button),  LV_STATE_PRESSED, 10);
  lv_style_set_shadow_ofs_y(&(smart_ring_ui_controller.styles.boot_button),  LV_STATE_PRESSED, 5);

  /* Comunication menu */
  lv_style_init(&(smart_ring_ui_controller.styles.boot_communications));
  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.boot_communications),   LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_text_color(&(smart_ring_ui_controller.styles.boot_communications), LV_STATE_DEFAULT, LV_COLOR_WHITE);

  /* Header and Footer */
  lv_style_init(&(smart_ring_ui_controller.styles.header_and_footer));
  lv_style_set_border_width(&(smart_ring_ui_controller.styles.header_and_footer), LV_STATE_DEFAULT, 0);
  lv_style_set_radius(&(smart_ring_ui_controller.styles.header_and_footer),       LV_STATE_DEFAULT, 0);
  lv_style_set_bg_color(&(smart_ring_ui_controller.styles.header_and_footer),     LV_STATE_DEFAULT, LV_COLOR_MAKE(0xEB, 0xED, 0xF0));

  /* FONTS */
  // 12 Normal
  lv_style_init(&(smart_ring_ui_controller.styles.font_12_normal));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_12_normal), LV_STATE_DEFAULT, &sr_font_montserrat_12);

  // 14 Normal
  lv_style_init(&(smart_ring_ui_controller.styles.font_14_normal));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_14_normal), LV_STATE_DEFAULT, &sr_font_montserrat_14);

  // 16 Normal
  lv_style_init(&(smart_ring_ui_controller.styles.font_16_normal));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_16_normal), LV_STATE_DEFAULT, &sr_font_montserrat_16);

  // 16 Bold
  lv_style_init(&(smart_ring_ui_controller.styles.font_16_bold));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_16_bold),   LV_STATE_DEFAULT, &sr_font_montserrat_16_bold);

  // 18 Normal
  lv_style_init(&(smart_ring_ui_controller.styles.font_18_normal));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_18_normal), LV_STATE_DEFAULT, &sr_font_montserrat_18);

  // 18 Bold
  lv_style_init(&(smart_ring_ui_controller.styles.font_18_bold));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_18_bold),   LV_STATE_DEFAULT, &sr_font_montserrat_18_bold);

  // 22 Normal
  lv_style_init(&(smart_ring_ui_controller.styles.font_22_normal));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_22_normal), LV_STATE_DEFAULT, &sr_font_montserrat_22);

  // 24 Normal
  lv_style_init(&(smart_ring_ui_controller.styles.font_24_normal));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_24_normal), LV_STATE_DEFAULT, &sr_font_montserrat_24);

  // 26 Normal
  lv_style_init(&(smart_ring_ui_controller.styles.font_26_normal));
  lv_style_set_text_font(&(smart_ring_ui_controller.styles.font_26_normal), LV_STATE_DEFAULT, &sr_font_montserrat_26);

  // Start the deliveries array with 1 delivery
  smart_ring_ui_controller.deliveries = (struct smart_ring_ui_delivery_t *)malloc(sizeof(struct smart_ring_ui_delivery_t));

  // Create the timer instance to be used
  const esp_timer_create_args_t ui_controller_timer_args = {
      .callback = &ui_callback_timer_callback,
      /* argument specified here will be passed to timer callback function */
      .name = "ui_controller_timer"
  };

  esp_timer_create(&ui_controller_timer_args, &(smart_ring_ui_controller.timer));

  const esp_timer_create_args_t ui_controller_standby_timer_args = {
      .callback = &ui_callback_standby_timer_callback,
      /* argument specified here will be passed to timer callback function */
      .name = "ui_standby_timer"};
  esp_timer_create(&ui_controller_standby_timer_args, &(smart_ring_ui_controller.standby_timer));
  return ESP_OK;
}

static void smart_ring_ui_select_menu(int menu_id) {

#ifndef NDEBUG
  printf("Changing screen to menu %d\n", menu_id);
#endif

  lv_obj_t *old_screen = lv_scr_act();
  int old_screen_id    = smart_ring_ui_controller.menu;

  smart_ring_ui_controller.screen = lv_obj_create(NULL, NULL);
  smart_ring_ui_controller.menu   = menu_id;

  lv_obj_add_style(smart_ring_ui_controller.screen, LV_OBJ_PART_MAIN, &(smart_ring_ui_controller.styles.blank_page));
  
  switch (menu_id) {
  case MENU_ID_BOOT:
    create_booting_up_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_MAIN:
    main_create_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_DASHBOARD:
    main_create_dashboard_menu(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_SELECTED_COMMUNICATION:
    create_selected_communication(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_SELECT_COMMUNICATION:
    create_communication_selection(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_PIN:
    create_insert_pin_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_SUPPORT_REVIEW:
    support_create_review(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_SUPPORT_SUBMITED:
    support_create_submited(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_CHANGED_STOCK:
    stock_update_success_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_SETTINGS:
    main_create_debug_menu(smart_ring_ui_controller.screen,  &smart_ring_ui_controller);
    break;
  case MENU_ID_CALIBRATION_EMPTY:
    calibration_create_no_bottle_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_CALIBRATION_FULL:
    calibration_create_full_bottle_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_CALIBRATION_PROGRESS:
    calibration_create_progress_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_CALIBRATION_FINISHED:
    calibration_create_finished_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_INFO:
    main_create_info_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_NEW_ORDER:
    order_create_new_order_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_REVIEW_ORDER:
    order_create_review_order_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_PROCESSING_ORDER:
    order_create_processing_order_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_SUCCESSFULL_ORDER:
    order_create_successfull_order_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_CANCELED_ORDER:
    order_create_canceled_order_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_ERROR_ORDER:
    order_create_error_order_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_ERROR_MANAGE:
    order_create_error_manage_order_screen(smart_ring_ui_controller.screen,  &smart_ring_ui_controller);
    break;
  case MENU_ID_SUCCESSFULL_MODE_CHANGE:
    order_create_successfull_change_mode_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_MANAGE_ORDERS:
    order_create_manage_order_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_CHANGE_ORDER_MODE:
    order_create_change_order_mode_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_RESET:
    main_create_reset_menu(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_RESET_DEVICE:
    reset_create_device_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_RESET_WIFI:
    reset_create_wifi_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_RESET_PROGRESS:
    reset_create_progress_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_RESET_SUCCESS:
    reset_create_finished_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller, true);
    break;
  case MENU_ID_RESET_FAILED:
    reset_create_finished_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller, false);
    break;
  case MENU_ID_UPDATE:
    update_create_option_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_FIRMWARE_UPDATE:
    update_create_firmware_progress_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_FIRMWARE_UPDATE_COMPLETE:
    update_create_firmware_finished_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller, true);
    break;
  case MENU_ID_FIRMWARE_UPDATE_FAILED:
    update_create_firmware_finished_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller, false);
    break;
  case MENU_ID_SLEEP:
    smart_ring_ui_create_sleep_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  case MENU_ID_POPUP_PENDING_ORDER:
    order_popup_pending_order_screen(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
    break;
  default:
    smart_ring_ui_controller.screen = old_screen;
    smart_ring_ui_controller.menu   = old_screen_id;
    break;
  }

#ifndef NDEBUG
  printf("Finished the creation of the display\n");
#endif

  lv_scr_load(smart_ring_ui_controller.screen);
  lv_disp_trig_activity(NULL);

#ifndef NDEBUG
  printf("Finish loading the menu, current screen %d\n",
         smart_ring_ui_controller.menu);
#endif

  lv_obj_del(old_screen);
}

char *smart_ring_ui_get_firwmare(void)
{
  return smart_ring_ui_controller.firmware_version;
}

void smart_ring_ui_set_firmware(char *version)
{
  strcpy(smart_ring_ui_controller.firmware_version, version);
}

char *smart_ring_ui_get_device_name(void)
{
  return smart_ring_ui_controller.device_name;
}

void smart_ring_ui_set_device_name(char *name)
{
  strcpy(smart_ring_ui_controller.device_name, name);

  if (smart_ring_ui_controller.menu == MENU_ID_MAIN)
  {
    main_update_device_name(name);
  }
}

char *smart_ring_ui_get_mac_address(void)
{
  return smart_ring_ui_controller.mac_address;
}

void smart_ring_ui_set_mac_address(char *name)
{
  strcpy(smart_ring_ui_controller.mac_address, name);
}

char *smart_ring_ui_get_ssid(void) { return smart_ring_ui_controller.ssid; }

void smart_ring_ui_set_ssid(char *name)
{
#ifndef NDEBUG
  ESP_LOGI(TAG, "Updating SSID : %s", name);
#endif
  strcpy(smart_ring_ui_controller.ssid, name);
}

char smart_ring_ui_get_order_mode(void)
{
  return smart_ring_ui_controller.order_mode;
}

void smart_ring_ui_set_order_mode(char mode)
{
  smart_ring_ui_controller.order_mode = mode;
  if (smart_ring_ui_controller.menu == MENU_ID_MAIN)
  {
    main_update_order_mode(mode);
  }
}

char smart_ring_ui_get_connection_type(void)
{
  return smart_ring_ui_controller.connection_type;
}

void smart_ring_ui_set_connection_type(char mode)
{
  smart_ring_ui_controller.connection_type = mode;
}

void smart_ring_ui_set_calibration(int no_deposit, int full_deposit, bool is_stable)
{
  smart_ring_ui_controller.no_deposit = no_deposit;
  smart_ring_ui_controller.full_deposit = full_deposit;
  smart_ring_ui_controller.is_stable = is_stable;
}

void smart_ring_ui_set_current_deposit(int current_deposit)
{
  smart_ring_ui_controller.current_deposit = current_deposit;

  if (smart_ring_ui_controller.menu == MENU_ID_MAIN)
  {
    main_update_bottle_icon(&smart_ring_ui_controller);
  }
}

uint8_t smart_ring_ui_get_rssi_level()
{
  return smart_ring_ui_controller.rssi_level;
}

void smart_ring_ui_set_rssi_level(uint8_t rssi_level)
{
#ifndef NDEBUG
  ESP_LOGI(TAG, "Changing UI RSSI");
#endif
  smart_ring_ui_controller.rssi_level = rssi_level;

  if (smart_ring_ui_controller.menu == MENU_ID_MAIN)
  {
#ifndef NDEBUG
    ESP_LOGI(TAG, "Updating signal icon");
#endif
    main_update_rssi_signal(rssi_level);
  }
}

void smart_ring_ui_set_stock(int stock)
{
  smart_ring_ui_controller.stock = stock;
  if (smart_ring_ui_controller.menu == MENU_ID_MAIN || smart_ring_ui_controller.state == STATE_13)
  {
    main_update_stock(stock);
  }
}

void create_header(lv_obj_t *parent, char *title, struct smart_ring_ui_controller_t *ui_controller)
{
  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *header = lv_cont_create(parent, NULL);
  lv_obj_add_style(header, LV_CONT_PART_MAIN, &(ui_controller->styles.header_and_footer));
  lv_obj_add_style(header, LV_CONT_PART_MAIN, &style_blue_text);
  lv_obj_set_size(header, 320, 30);
  lv_obj_align(header, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  lv_obj_t *label = lv_label_create(header, NULL);
  lv_label_set_text(label, title);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

void smart_ring_ui_check_flags()
{

  int retries = 0;
  if (!smart_ring_ui_controller.flags.has_flags)
  {
    return;
  }

  if (smart_ring_ui_controller.flags.flag.check_option)
  {
    switch (smart_ring_ui_controller.state)
    {
    case STATE_13:
      modal_create_template(lv_scr_act(), &smart_ring_ui_controller, "Stock", "A enviar pedido", true);
      smart_ring_ui_controller.flags.flag.change_stock = true;
      break;
    case STATE_14:
      smart_ring_ui_update_state(STATE_11);
      break;
    case STATE_16:
      modal_create_template(lv_scr_act(), &smart_ring_ui_controller, "Suporte", "A enviar pedido", true);
      smart_ring_ui_controller.flags.flag.send_ticket = true;
      break;
    case STATE_17:
      smart_ring_ui_update_state(STATE_11);
      break;
    case STATE_19:
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      smart_ring_ui_update_state(STATE_11);
      break;
    case STATE_21:
      smart_ring_ui_update_state(STATE_11);
      break;
    default:
      break;
    }
    smart_ring_ui_controller.flags.flag.check_option = false;
  }

  if (smart_ring_ui_controller.flags.flag.cancel_update)
  {
    smart_ring_ui_select_menu(MENU_ID_FIRMWARE_UPDATE_FAILED);
    smart_ring_ui_controller.flags.flag.cancel_update = false;
  }

  if (smart_ring_ui_controller.flags.flag.success)
  {
    smart_ring_ui_select_menu(MENU_ID_FIRMWARE_UPDATE_COMPLETE);
    smart_ring_ui_controller.flags.flag.success = false;
  }

  if (smart_ring_ui_controller.flags.flag.touch_pressed)
  {

#ifndef NDEBUG
    ESP_LOGI(TAG, "Standby timer : %s", esp_timer_is_active(smart_ring_ui_controller.standby_timer) ? "ON" : "NOK");
#endif
    // Get if any timer is active
    if (esp_timer_is_active(smart_ring_ui_controller.standby_timer))
    {

      // Stop the timer
      esp_timer_stop(smart_ring_ui_controller.standby_timer);

      // Get the timer type
      if (smart_ring_ui_controller.standby_timer_type == STANDBY)
      {
        smart_ring_ui_controller.standby_controller.standby_active = true;
      }
      else if (smart_ring_ui_controller.standby_timer_type == SLEEP)
      {
        smart_ring_ui_controller.standby_controller.sleep_active = true;
      }
      else
      {
        smart_ring_ui_controller.standby_controller.standby_dashboard_active = true;
      }

#ifndef NDEBUG
      ESP_LOGI(TAG, "Stopping standby timers\n\tSleep     : %s\n\tStandby   : %s\n\tDashboard : %s",
               smart_ring_ui_controller.standby_controller.sleep_active ? "ON" : "NOK",
               smart_ring_ui_controller.standby_controller.standby_active ? "ON" : "NOK",
               smart_ring_ui_controller.standby_controller.standby_dashboard_active ? "ON" : "NOK");
#endif
    }
    smart_ring_ui_controller.flags.flag.touch_pressed = false;
  }

  if (smart_ring_ui_controller.flags.flag.touch_released)
  {

    // Get if the touch was for an action
    if (!smart_ring_ui_controller.standby_controller.has_callback)
    {
#ifndef NDEBUG
      ESP_LOGI(TAG, "Has no callback, check for timers\n\tSleep     : %s\n\tStandby   : %s\n\tDashboard : %s",
               smart_ring_ui_controller.standby_controller.sleep_active ? "ON" : "NOK",
               smart_ring_ui_controller.standby_controller.standby_active ? "ON" : "NOK",
               smart_ring_ui_controller.standby_controller.standby_dashboard_active ? "ON" : "NOK");
#endif
      if (smart_ring_ui_controller.standby_controller.sleep_active)
      {
        smart_ring_ui_controller.standby_timer_type = SLEEP;
        esp_timer_start_once(smart_ring_ui_controller.standby_timer, SLEEP_TIMEOUT * 1000000);
      }
      else if (smart_ring_ui_controller.standby_controller.standby_active)
      {
        smart_ring_ui_controller.standby_timer_type = STANDBY;
        esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      }
      else if (smart_ring_ui_controller.standby_controller.standby_dashboard_active)
      {
        smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
        esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      }

#ifndef NDEBUG
      ESP_LOGI(TAG, "Timers restarted");
#endif
    }

    smart_ring_ui_controller.standby_controller.has_callback = false;
    smart_ring_ui_controller.standby_controller.sleep_active = false;
    smart_ring_ui_controller.standby_controller.standby_active = false;
    smart_ring_ui_controller.standby_controller.standby_dashboard_active = false;
    smart_ring_ui_controller.flags.flag.touch_released = false;
  }

  if (smart_ring_ui_controller.flags.flag.update_state)
  {
    switch (smart_ring_ui_controller.state)
    {
    case STATE_0:
      smart_ring_ui_select_menu(MENU_ID_BOOT);
      break;
    case STATE_1:
      smart_ring_ui_select_menu(MENU_ID_SELECT_COMMUNICATION);
      break;
    case STATE_2:
      smart_ring_ui_select_menu(MENU_ID_SELECTED_COMMUNICATION);
      smart_ring_ui_controller.timer_type = BOOT_START_COMMUNICATION;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 5);
      break;
    case STATE_3:
      modal_create_provision(smart_ring_ui_controller.screen, &smart_ring_ui_controller);
      break;
    case STATE_4:
      boot_update_connection_screen();
      break;
    case STATE_5:
      smart_ring_ui_controller.is_admin = false;
      smart_ring_ui_controller.is_master = false;
      smart_ring_ui_select_menu(MENU_ID_MAIN);
      smart_ring_ui_controller.standby_timer_type = SLEEP;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, SLEEP_TIMEOUT * 1000000);
      break;
    case STATE_6:
      modal_create_template(smart_ring_ui_controller.screen, &smart_ring_ui_controller, "Erro", "Sem ligação à internet", true);
      modal_complete_progress(&smart_ring_ui_controller, false, "Sem ligação à internet");

      // Create the timer to close the modal in 3 seconds
      smart_ring_ui_controller.timer_type = CLOSE_MODAL;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 3);
      smart_ring_ui_controller.state = smart_ring_ui_controller.old_state;
      break;
    case STATE_7:
      smart_ring_ui_select_menu(MENU_ID_PIN);
      smart_ring_ui_controller.standby_timer_type = STANDBY;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_8:
#ifndef NDEBUG
      ESP_LOGI(TAG, "Inserted pin: %s", pin_get_inserted_pin());
#endif
      // Create the modal
      modal_create_template(smart_ring_ui_controller.screen, &smart_ring_ui_controller, "Validação de pin", "A validar", true);
      smart_ring_ui_controller.flags.flag.sign_in = true;

      // Start the timer for the timeout
      smart_ring_ui_controller.timer_type = LOGIN_VALIDATION;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 5);
      break;
    case STATE_9:
      modal_complete_progress(&smart_ring_ui_controller, false, "Erro ao validar pin");

      // Create the timer to close the modal in 3 seconds
      smart_ring_ui_controller.timer_type = CLOSE_MODAL;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 3);
      smart_ring_ui_controller.state = STATE_7;
      break;
    case STATE_10:
      modal_complete_progress(&smart_ring_ui_controller, true, "Validado com sucesso");

      // Create the timer to close the modal in 3 seconds
      smart_ring_ui_controller.timer_type = LOGIN_SUCCESSFULL;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 3);
      break;
    case STATE_11:
      smart_ring_ui_select_menu(MENU_ID_DASHBOARD);
      smart_ring_ui_controller.standby_timer_type = STANDBY;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_12:
      smart_ring_ui_select_menu(MENU_ID_INFO);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_13:
      // TABVIEW NOTHING TO BE MADE
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_14:
      smart_ring_ui_select_menu(MENU_ID_CHANGED_STOCK);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_15:
      // TABVIEW NOTHING TO BE MADE
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_16:
      smart_ring_ui_select_menu(MENU_ID_SUPPORT_REVIEW);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_17:
      smart_ring_ui_select_menu(MENU_ID_SUPPORT_SUBMITED);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_18:
      smart_ring_ui_select_menu(MENU_ID_MANAGE_ORDERS);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_19:
      smart_ring_ui_select_menu(MENU_ID_ERROR_MANAGE);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_20:
      smart_ring_ui_select_menu(MENU_ID_CHANGE_ORDER_MODE);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_21:
      smart_ring_ui_select_menu(MENU_ID_SUCCESSFULL_MODE_CHANGE);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_22:
      smart_ring_ui_controller.order.bottles = 0;
      smart_ring_ui_controller.order.cups = 0;
      smart_ring_ui_controller.order.changing_bottles = true;
      smart_ring_ui_select_menu(MENU_ID_NEW_ORDER);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_23:
      smart_ring_ui_select_menu(MENU_ID_REVIEW_ORDER);
      break;
    case STATE_24:
      smart_ring_ui_controller.order.confirmation = 'x';
      smart_ring_ui_select_menu(MENU_ID_CANCELED_ORDER);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_25:
      smart_ring_ui_controller.order.confirmation = 'a';
      smart_ring_ui_controller.timer_type = CONFIRM_ORDER;
      esp_timer_start_once(smart_ring_ui_controller.timer, 5 * 1000000);
      smart_ring_ui_select_menu(MENU_ID_PROCESSING_ORDER);
      smart_ring_ui_controller.flags.flag.confirm_order = true;
      break;
    case STATE_26:
      smart_ring_ui_select_menu(MENU_ID_ERROR_ORDER);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_27:
      esp_timer_stop(smart_ring_ui_controller.timer);
      smart_ring_ui_select_menu(MENU_ID_SUCCESSFULL_ORDER);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_28:
      smart_ring_ui_select_menu(MENU_ID_SETTINGS);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_29:
      smart_ring_ui_select_menu(MENU_ID_CALIBRATION_EMPTY);
      break;
    case STATE_30:
      smart_ring_ui_select_menu(MENU_ID_CALIBRATION_PROGRESS);
      smart_ring_ui_calibration_update_progress_screen(false);
      break;
    case STATE_31:
      smart_ring_ui_calibration_update_progress_screen(true);
      if (smart_ring_ui_get_controller()->calibration_step == 1)
      {
        smart_ring_ui_controller.timer_type = CALIBRATION_EMPTY;
        esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 3);
      }
      else
      {
        smart_ring_ui_controller.timer_type = CALIBRATION_FULL;
        esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 3);
      }
      break;
    case STATE_32:
      smart_ring_ui_select_menu(MENU_ID_CALIBRATION_FULL);
      break;
    case STATE_33:
      smart_ring_ui_select_menu(MENU_ID_CALIBRATION_FINISHED);
      break;
    case STATE_34:
      smart_ring_ui_select_menu(MENU_ID_FIRMWARE_UPDATE);
      break;
    case STATE_35:
      smart_ring_ui_controller.flags.flag.order_popup_message = true;
      // Create the modal
      modal_create_template(smart_ring_ui_controller.screen, &smart_ring_ui_controller, "Nova encomenda", "a verificar pedidos pendentes", true);

      // Start the timer for the timeout

      smart_ring_ui_controller.timer_type = PENDING_ORDERS;
      esp_timer_start_once(smart_ring_ui_controller.timer, 5 * 1000000);
      break;
    case STATE_36:
      smart_ring_ui_select_menu(MENU_ID_RESET);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_37:
      smart_ring_ui_select_menu(MENU_ID_RESET_DEVICE);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_38:
      smart_ring_ui_select_menu(MENU_ID_RESET_WIFI);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_39:
      smart_ring_ui_select_menu(MENU_ID_RESET_PROGRESS);
      break;
    case STATE_40:
      smart_ring_ui_select_menu(MENU_ID_RESET_SUCCESS);
      break;
    case STATE_41:
      smart_ring_ui_select_menu(MENU_ID_RESET_FAILED);
      break;
    case STATE_42:
      smart_ring_ui_select_menu(MENU_ID_SLEEP);
      gpio_set_level(CONFIG_LV_DISP_PIN_BCKL, 0);
      break;
    case STATE_43:
      smart_ring_ui_select_menu(MENU_ID_UPDATE);
      break;
    case STATE_44:
#ifndef NDEBUG
      ESP_LOGI(TAG, "Inserted change pin: %s", pin_get_inserted_pin());
#endif
      // Create the modal
      modal_create_template(smart_ring_ui_controller.screen, &smart_ring_ui_controller, "Atualização PIN",
                            "A alterar pin...", true);

      smart_ring_ui_controller.flags.flag.pin_changed = true;

      // Start the timer for the timeout
      smart_ring_ui_controller.timer_type = PIN_UPDATION;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 5);
      break;

    case STATE_45:
      modal_complete_progress(&smart_ring_ui_controller, false, "Erro a atualizar pin");

      // Create the timer to close the modal in 3 seconds
      smart_ring_ui_controller.timer_type = CLOSE_MODAL;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 3);
      smart_ring_ui_get_controller()->flags.flag.update_pin = true;
      smart_ring_ui_update_state(STATE_7);
      break;

    case STATE_46:
      modal_complete_progress(&smart_ring_ui_controller, false, "Erro ao obter pedidos pendentes");

      // Create the timer to close the modal in 3 seconds
      smart_ring_ui_controller.timer_type = CLOSE_MODAL;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 3);
      break;

    case STATE_47:
      smart_ring_ui_select_menu(MENU_ID_POPUP_PENDING_ORDER);
      smart_ring_ui_controller.standby_timer_type = STANDBY_DASHBOARD;
      esp_timer_start_once(smart_ring_ui_controller.standby_timer, STANDBY_TIMEOUT * 1000000);
      break;
    case STATE_48:
      // Create the modal
      modal_create_template(smart_ring_ui_controller.screen, &smart_ring_ui_controller, "Conexão perdida ",
                            "A reiniciar o dispositivo", true);

      // Create the timer to close the modal in 3 seconds
      smart_ring_ui_controller.timer_type = CLOSE_MODAL;

      smart_ring_ui_get_controller()->flags.flag.reset_device = true;
      esp_timer_start_once(smart_ring_ui_controller.timer, 1000000 * 5);
      break;
    default:
      break;
    }
    smart_ring_ui_controller.flags.flag.update_state = false;
  }
}

void smart_ring_ui_update_state(enum smart_ring_ui_state_machine_t state)
{
#ifndef NDEBUG
  ESP_LOGI(TAG, "Updating state %d - %d", smart_ring_ui_controller.state, state);
#endif
  smart_ring_ui_controller.old_state = smart_ring_ui_controller.state;
  smart_ring_ui_controller.state = state;
  smart_ring_ui_controller.flags.flag.update_state = true;
}

struct smart_ring_ui_controller_t *smart_ring_ui_get_controller()
{
  return &smart_ring_ui_controller;
}

static void ui_callback_standby_timer_callback(void *arg)
{

  switch (smart_ring_ui_controller.standby_timer_type)
  {
  case SLEEP:
#ifndef NDEBUG
    ESP_LOGI(TAG, "Sleep timer activated");
#endif
    smart_ring_ui_update_state(STATE_42);
    break;
  case STANDBY:
#ifndef NDEBUG
    ESP_LOGI(TAG, "Standby timer activated");
#endif
    smart_ring_ui_update_state(STATE_5);
    break;
  case STANDBY_DASHBOARD:
#ifndef NDEBUG
    ESP_LOGI(TAG, "Standby dashboard timer activated");
#endif
    smart_ring_ui_update_state(STATE_11);
    break;
  default:
    ESP_LOGE(TAG, "Invalid timer type");
    break;
  }
}

static void ui_callback_timer_callback(void *arg)
{

  switch (smart_ring_ui_controller.timer_type)
  {
  case CLOSE_MODAL:
#ifndef NDEBUG
    printf("Closing modal\n");
#endif
    // Since the modal is the last child, use that position to delete the
    // object
    lv_obj_del(lv_obj_get_child(smart_ring_ui_controller.screen, NULL));
    break;
  case BOOT_START_COMMUNICATION:
#ifndef NDEBUG
    ESP_LOGI(TAG, "Starting communication");
#endif
    smart_ring_ui_controller.flags.flag.start_communication = true;
    break;
  case LOGIN_VALIDATION:
    smart_ring_ui_update_state(STATE_9);
    break;
  case LOGIN_SUCCESSFULL:
    smart_ring_ui_update_state(STATE_11);
    break;
  case REQUESTING_ORDERS:
    smart_ring_ui_update_state(STATE_19);
    break;
  case CALIBRATION_EMPTY:
    smart_ring_ui_update_state(STATE_32);
    break;
  case CALIBRATION_FULL:
    smart_ring_ui_update_state(STATE_33);
    break;
  case CONFIRM_ORDER:
    smart_ring_ui_update_state(STATE_26);
    break;
  case PIN_UPDATION:
    smart_ring_ui_update_state(STATE_45);
  case PENDING_ORDERS:
    smart_ring_ui_update_state(STATE_11);
  default:
    break;
  }
}

float map(int value_to_map, int input_range_start, int input_range_end,
          int output_range_start, int output_range_end)
{
  float mapped_value = (float)(value_to_map - input_range_start) * (output_range_end - output_range_start) /
                           (input_range_end - input_range_start) +
                       output_range_start;

  if (mapped_value > output_range_end)
  {
    mapped_value = output_range_end;
  }
  else if (mapped_value < output_range_start)
  {
    mapped_value = output_range_start;
  }

  return mapped_value;
}
