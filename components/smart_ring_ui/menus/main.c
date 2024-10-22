/**
 * @file main.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Responsible for the main screen information and the update of it
 * @version 2.0
 * @date 2022-06-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "main.h"
#include "libs.h"

// Tag for logging to the monitor
static const char *TAG = "UI_MAIN";

LV_IMG_DECLARE(icon_device)
LV_IMG_DECLARE(icon_stock)
LV_IMG_DECLARE(icon_manual)
LV_IMG_DECLARE(icon_auto)
LV_IMG_DECLARE(icon_truck)
LV_IMG_DECLARE(icon_signin)
LV_IMG_DECLARE(icon_logout)
LV_IMG_DECLARE(icon_info)
LV_IMG_DECLARE(icon_cog)
LV_IMG_DECLARE(wifi_signal_0)
LV_IMG_DECLARE(wifi_signal_1)
LV_IMG_DECLARE(wifi_signal_2)
LV_IMG_DECLARE(wifi_signal_3)
LV_IMG_DECLARE(cbre_logo)
LV_IMG_DECLARE(fonte_logo)
LV_IMG_DECLARE(icon_bottle_25)
LV_IMG_DECLARE(icon_bottle_50)
LV_IMG_DECLARE(icon_bottle_75)
LV_IMG_DECLARE(icon_bottle_100)
LV_IMG_DECLARE(icon_bottle_full_inv)
LV_IMG_DECLARE(icon_home)
LV_IMG_DECLARE(icon_stable)
LV_IMG_DECLARE(icon_unstable)
LV_IMG_DECLARE(icon_down_arrow)

/* Support options objects */

static lv_obj_t *label_support_option_1 = NULL;
static lv_obj_t *label_support_option_2 = NULL;

// Static object variables for screen information update
// Main screen variables
static lv_obj_t *com_signal_img    = NULL;
static lv_obj_t *device_name_label = NULL;
static lv_obj_t *eta_label         = NULL;
static lv_obj_t *main_bottle_img   = NULL;
static lv_obj_t *label_stock       = NULL;
static lv_obj_t *delivery_mode_img = NULL;

// Support variables
static struct smart_ring_support_option_t support_options[] = {
    {.type = BAD_TASTE_WATER, .text = "Água com mau sabor"},
    {.type = BOTH_TAPS_COLD,  .text = "Faz frio nas duas torneiras"},
    {.type = NO_COLD,         .text = "Não faz frio"},
    {.type = LEAKING_WATER,   .text = "Verte água"},
    {.type = OTHER,           .text = "Outro"}
};

static int     current_page  = 1;
static uint8_t num_options   = sizeof(support_options) / sizeof(struct smart_ring_support_option_t);
static uint8_t support_pages = 1;

/* Handler functions*/

static void main_support_view_option_select_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
#ifndef NDEBUG

    struct smart_ring_support_option_t *option = lv_obj_get_user_data(button);
    printf("Support type: "
           "%d\n",
           option->type);
#endif

    // Get the support type
    switch (option->type) {
    case BAD_TASTE_WATER: printf("Água com mau sabor\n");          break;
    case BOTH_TAPS_COLD:  printf("Faz frio nas duas torneiras\n"); break;
    case NO_COLD:         printf("Não faz frio\n");                break;
    case LEAKING_WATER:   printf("Verte água\n");                  break;
    case OTHER:           printf("Outro\n");                       break;
    default:
      break;
    }

    smart_ring_ui_get_controller()->support_type = option->type;
    smart_ring_ui_update_state(STATE_16);
  }
}


static void main_support_view_next_page_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;

#ifndef NDEBUG
    printf("Support: Next page\n");
#endif

    if (current_page == support_pages) {
      return;
    }
    current_page++;
    lv_label_set_text(label_support_option_1, support_options[current_page * 2 - 2].text);
    lv_obj_set_user_data(lv_obj_get_parent(label_support_option_1), &(support_options[current_page * 2 - 2]));

    if (current_page * 2 - 1 == num_options) {
      lv_obj_t *parent_button = lv_obj_get_parent(label_support_option_2);
      lv_obj_set_hidden(parent_button, true);
    } 
    else {
      lv_label_set_text(label_support_option_2, support_options[current_page * 2 - 1].text);
      lv_obj_set_user_data(lv_obj_get_parent(label_support_option_2), &(support_options[current_page * 2 - 1]));
    }
  }
}


static void main_support_view_previous_page_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;

#ifndef NDEBUG
    printf("Support: Previous page\n");
#endif

    if (current_page == 1) {
      return;
    }

    if (current_page == support_pages) {
      // Check if the second button is hidden
      lv_obj_t *parent_button = lv_obj_get_parent(label_support_option_2);

      if (lv_obj_get_hidden(parent_button)) {
        lv_obj_set_hidden(parent_button, false);
      }
    }
    current_page--;
    lv_label_set_text(label_support_option_1, support_options[current_page * 2 - 2].text);
    lv_obj_set_user_data(lv_obj_get_parent(label_support_option_1), &(support_options[current_page * 2 - 2]));
    lv_label_set_text(label_support_option_2, support_options[current_page * 2 - 1].text);
    lv_obj_set_user_data(lv_obj_get_parent(label_support_option_2), &(support_options[current_page * 2 - 1]));
  }
}


static void main_sign_in_event_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_get_controller()->flags.flag.start_login          = true;
  }
}


static void main_update_event_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_34);

    smart_ring_ui_get_controller()->flags.flag.update_firmware      = true;
  }
}


void main_logout_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    // TODO : Send the disconnect event on mqtt
    smart_ring_ui_update_state(STATE_5);
  }
}


void main_home_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_11);
  }
}


static void order_tab_information_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_12);
  }
}


static void order_tab_settings_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_28);
  }
}


static void order_tab_order_mode_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_20);
  }
}


static void order_tab_manage_order_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_get_controller()->flags.flag.manage_orders        = true;
    modal_create_template(lv_scr_act(), smart_ring_ui_get_controller(), "Encomendas", "A verificar encomendas", true);

    smart_ring_ui_get_controller()->timer_type = REQUESTING_ORDERS;
    esp_timer_start_once(smart_ring_ui_get_controller()->timer, 1000000 * 10);
  }
}

static void stock_tab_add_bottle_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
#ifndef NDEBUG
    ESP_LOGI(TAG, "Adding new stock bottle");
#endif
    int stock = atoi(lv_label_get_text(label_stock));
#ifndef NDEBUG
    ESP_LOGI(TAG, "Updating bottle stock %d", stock);
#endif
    main_update_stock(stock + 1);
    smart_ring_ui_get_controller()->updated_stock  = stock+1;
  }
}


static void stock_tab_remove_bottle_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    int stock = atoi(lv_label_get_text(label_stock));
    main_update_stock(stock == 0 ? 0 : stock - 1);
  }
}


static void smart_ring_ui_debug_restart_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_36);
  }
}


static void smart_ring_ui_restart_device_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_37);
  }
}


static void smart_ring_ui_restart_wifi_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_38);
  }
}


static void smart_ring_ui_debug_update_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_43);
    smart_ring_ui_get_controller()->flags.flag.update = true;
  }
}


void smart_ring_ui_check_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_get_controller()->flags.flag.check_option = true;

  }
}


static void smart_ring_ui_main_new_order_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_35);
  }
}


static bool is_stock_tabview = false;
static void main_tabview_screen_change_handler(lv_obj_t *tabview, lv_event_t event) {
  if (event != LV_EVENT_VALUE_CHANGED)
    return;

  lv_tabview_ext_t *ext = (lv_tabview_ext_t *)lv_obj_get_ext_attr(tabview);
  const char *tab_name  = ext->tab_name_ptr[lv_tabview_get_tab_act(tabview)];

  if (strcmp(tab_name, "Encomendas") == 0 || strcmp(tab_name, "Suporte") == 0) {

    if (strcmp(tab_name, "Suporte") == 0) {
      smart_ring_ui_get_controller()->state = 15;
    } else {
      smart_ring_ui_get_controller()->state = 11;
    }
    lv_label_set_text_fmt(label_stock, "%d", smart_ring_ui_get_controller()->stock);
    is_stock_tabview = false;
  } else {
    smart_ring_ui_get_controller()->state = 13;
    is_stock_tabview = true;
  }
  printf("Current Active Tab : %s\n", tab_name);
}


void smart_ring_ui_main_update_stock_value(int stock) {
  lv_label_set_text_fmt(label_stock, "%d", stock);
}


void smart_ring_ui_main_clear_next_eta(void) {
  if (eta_label == NULL) {
    return;
  }

  lv_obj_set_style_local_text_color(eta_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x97, 0x97, 0x97));
  lv_label_set_text(
      eta_label, "Não existem encomendas pendentes                          "
                 "                                                      ");
  lv_label_set_long_mode(eta_label, LV_LABEL_LONG_SROLL_CIRC);
}


void smart_ring_ui_main_update_next_eta(void) {

#ifndef NDEBUG
  ESP_LOGI(TAG, "Getting next deliver eta");
#endif

  int day = -1, month = -1, delivery = 0;
  struct smart_ring_ui_controller_t *ui_controller = smart_ring_ui_get_controller();

  for (size_t i = 0; i < ui_controller->number_of_deliveries; i++) {
    int new_day, new_month;
    char temp[5];

    // Get day
    sprintf(temp, "%c%c", ui_controller->deliveries[i].date[8], ui_controller->deliveries[i].date[9]);
    new_day    = atoi(temp);

    // Get month
    sprintf(temp, "%c%c", ui_controller->deliveries[i].date[5], ui_controller->deliveries[i].date[6]);
    new_month  = atoi(temp);

    if ((day == -1 || month == -1) || (new_month < month) || (new_month == month && new_day < day)) {
      day      = new_day;
      month    = new_month;
      delivery = i;
    }
  }

#ifndef NDEBUG
  ESP_LOGI(TAG,
           "Next deliver ETA\n\tBottles : %d\n\tCups : %d\n\tOrdered : "
           "%s\n\tETA : %s\n\tState : %s\n",
           ui_controller->deliveries[delivery].bottles,
           ui_controller->deliveries[delivery].cups,
           ui_controller->deliveries[delivery].ordered_date,
           ui_controller->deliveries[delivery].date,
           ui_controller->deliveries[delivery].status);
#endif

  lv_obj_set_style_local_text_color(eta_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x33, 0x33, 0x33));
  lv_label_set_text_fmt(eta_label, "Próxima entrega : %d/%d", day, month);
  lv_label_set_long_mode(eta_label, LV_LABEL_LONG_EXPAND);
}

void main_create_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  lv_obj_add_style(parent, LV_PAGE_PART_BG, &(ui_controller->styles.blank_page));

  /*********
   * Header
   *********/
  lv_obj_t *header = lv_cont_create(parent, NULL);
  lv_obj_add_style(header, LV_CONT_PART_MAIN, &(ui_controller->styles.header_and_footer));
  lv_obj_set_size(header, 320, 30);
  lv_obj_align(header, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  // Icons
  lv_obj_t *img_var = lv_img_create(parent, NULL);
  lv_img_set_src(img_var, &icon_device);
  lv_obj_set_pos(img_var, 22, 5);

  img_var = lv_img_create(parent, NULL);
  lv_img_set_src(img_var, &icon_stock);
  lv_obj_set_pos(img_var, 200, 5);
  delivery_mode_img = lv_img_create(parent, NULL);
  lv_img_set_src(delivery_mode_img, &icon_manual);
  if (ui_controller->order_mode == 'a')  lv_img_set_src(delivery_mode_img, &icon_auto);
  lv_obj_set_pos(delivery_mode_img, 252, 3);

  // Signal image
  com_signal_img = lv_img_create(parent, NULL);
  switch (ui_controller->rssi_level) {
  case 0:
    lv_img_set_src(com_signal_img, &wifi_signal_0);
    break;
  case 1:
    lv_img_set_src(com_signal_img, &wifi_signal_1);
    break;
  case 2:
    lv_img_set_src(com_signal_img, &wifi_signal_2);
    break;
  case 3:
    lv_img_set_src(com_signal_img, &wifi_signal_3);
    break;
  default:
    lv_img_set_src(com_signal_img, &wifi_signal_0);
    break;
  }

  lv_img_set_zoom(com_signal_img, 90);
  lv_obj_align(com_signal_img, delivery_mode_img, LV_ALIGN_IN_RIGHT_MID, 50, 0);

  // Text
  device_name_label = lv_label_create(header, NULL);
  lv_label_set_recolor(device_name_label, true);
  lv_label_set_long_mode(device_name_label, LV_LABEL_LONG_SROLL_CIRC);

  char aux_name[64];
  sprintf(aux_name, "#979797 %s#", ui_controller->device_name);
  lv_label_set_text(device_name_label, aux_name);
  lv_obj_set_width(device_name_label, 130);
  lv_obj_align(device_name_label, NULL, LV_ALIGN_IN_LEFT_MID, 52, 0);
  lv_label_set_align(device_name_label, LV_LABEL_ALIGN_LEFT);

  char aux_stock[15];
  sprintf(aux_stock, "#979797 %d#", ui_controller->stock);

  label_stock = lv_label_create(header, NULL);
  lv_label_set_recolor(label_stock, true);
  lv_label_set_text(label_stock, aux_stock);
  lv_obj_set_width(label_stock, 14);
  lv_obj_align(label_stock, NULL, LV_ALIGN_IN_LEFT_MID, 222, 0);
  lv_label_set_align(label_stock, LV_LABEL_ALIGN_LEFT);

  /*********
   * Footer
   *********/
  lv_obj_t *footer = lv_cont_create(parent, NULL);
  lv_obj_add_style(footer, LV_CONT_PART_MAIN, &(ui_controller->styles.header_and_footer));
  lv_obj_set_size(footer, 360, 30);
  lv_obj_align(footer, parent, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
  lv_obj_set_pos(footer, 0, 210);

  img_var = lv_img_create(parent, NULL);
  lv_img_set_src(img_var, &icon_truck);
  lv_img_set_zoom(img_var, 118);
  lv_obj_align(img_var, footer, LV_ALIGN_IN_LEFT_MID, 8, 0);

  eta_label = lv_label_create(parent, NULL);
  lv_obj_set_style_local_text_color(eta_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x97, 0x97, 0x97));

  if (ui_controller->number_of_deliveries > 0) {
    smart_ring_ui_main_update_next_eta();
  } else {
    lv_label_set_text(
        eta_label, "Não existem encomendas pendentes                          "
                   "                                                      ");
  }
  lv_label_set_long_mode(eta_label, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_set_width(eta_label, 260);
  lv_obj_align(eta_label, footer, LV_ALIGN_IN_LEFT_MID, 50, 0);
  lv_obj_set_style_local_text_opa(eta_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_80);
  lv_obj_add_style(eta_label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_14_normal));

  /*****************
   * Fonte Viva Logo
   *****************/
  lv_obj_t *img_logo = lv_img_create(parent, NULL);
  lv_img_set_src(img_logo, &cbre_logo);
  lv_obj_set_pos(img_logo, 20, 60);

  /*****************
   * Sign in button
   *****************/
  static lv_style_t style_btn_green;
  lv_style_init(&style_btn_green);
  lv_style_set_bg_color(&style_btn_green, LV_STATE_DEFAULT, LV_COLOR_GREEN);
  lv_style_set_bg_color(&style_btn_green, LV_STATE_PRESSED, LV_COLOR_GREEN);
  lv_style_set_border_color(&style_btn_green, LV_STATE_PRESSED, LV_COLOR_LIME);
  lv_style_set_text_color(&style_btn_green, LV_STATE_PRESSED, LV_COLOR_WHITE);
  

  lv_obj_t *btn_sign_in = lv_btn_create(parent, NULL);
  lv_obj_add_style(btn_sign_in, LV_BTN_PART_MAIN, ui_controller->newVersion ? &style_btn_green : &(ui_controller->styles.blue_button));
  lv_obj_set_event_cb(btn_sign_in, ui_controller->newVersion ? main_update_event_handler : main_sign_in_event_handler);
  lv_obj_set_size(btn_sign_in, 140, 40);
  lv_obj_set_pos(btn_sign_in, 20, 140);

  lv_obj_t *label = lv_label_create(btn_sign_in, NULL);
  lv_label_set_text(label, ui_controller->newVersion ? "    Atualizar" : "  Aceder");

  if (ui_controller->newVersion) {
    lv_obj_set_style_local_bg_color(btn_sign_in,   LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_obj_set_style_local_text_color(btn_sign_in, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(btn_sign_in, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
  }
  ui_controller->newVersion ? lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold)) : 0;

  img_var = lv_img_create(parent, NULL);
  lv_img_set_src(img_var, &icon_signin);
  lv_obj_set_pos(img_var, 36, 150);

  /*********
   * Bottle
   *********/
  main_bottle_img = lv_img_create(parent, NULL);

  uint8_t percentage = map(ui_controller->current_deposit, ui_controller->no_deposit, ui_controller->full_deposit, 0, 100);

  if (percentage >= 75) {
    lv_img_set_src(main_bottle_img, &icon_bottle_100);
  } else if (percentage >= 50) {
    lv_img_set_src(main_bottle_img, &icon_bottle_75);
  } else if (percentage >= 25) {
    lv_img_set_src(main_bottle_img, &icon_bottle_50);
  } else {
    lv_img_set_src(main_bottle_img, &icon_bottle_25); 
  }
  lv_obj_set_pos(main_bottle_img, 220, 60);
}


static void main_create_order_view(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  lv_obj_add_style(parent, LV_PAGE_PART_BG, &(ui_controller->styles.blank_page));

  /****************************
   *  Manage delivery button  *
   ****************************/
  lv_obj_t *btn_manage_deliveries = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_manage_deliveries, order_tab_manage_order_handler);
  lv_obj_add_style(btn_manage_deliveries, LV_BTN_PART_MAIN, &(ui_controller->styles.white_button));
  lv_obj_set_size(btn_manage_deliveries, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT * 2 + 10);
  lv_obj_align(btn_manage_deliveries, parent, LV_ALIGN_CENTER, 0, -20);
  lv_btn_set_layout(btn_manage_deliveries, LV_LAYOUT_OFF);

  static lv_style_t style_blue_font;
  lv_style_init(&style_blue_font);
  lv_style_set_text_color(&style_blue_font, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *label = lv_label_create(btn_manage_deliveries, NULL);
  lv_label_set_text(label, "Gerir");
  lv_obj_align(label, btn_manage_deliveries, LV_ALIGN_CENTER, 0, -10);

  label = lv_label_create(btn_manage_deliveries, NULL);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_font);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_12_normal));
  lv_label_set_text(label, "Encomenda");
  lv_obj_align(label, btn_manage_deliveries, LV_ALIGN_CENTER, 0, 10);

  /*************************
   *  New delivery button  *
   *************************/
  lv_obj_t *btn_new_delivery = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_new_delivery, smart_ring_ui_main_new_order_handler);
  lv_obj_add_style(btn_new_delivery, LV_BTN_PART_MAIN, &(ui_controller->styles.white_button));
  lv_obj_set_size(btn_new_delivery, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT * 2 + 10);
  lv_obj_align(btn_new_delivery, btn_manage_deliveries, LV_ALIGN_OUT_LEFT_MID, -8, 0);
  lv_btn_set_layout(btn_new_delivery, LV_LAYOUT_OFF);

  label = lv_label_create(btn_new_delivery, NULL);
  lv_label_set_text(label, "Nova");
  lv_obj_align(label, btn_new_delivery, LV_ALIGN_CENTER, 0, -10);
  // lv_obj_set_pos(label, 0, 40);

  label = lv_label_create(btn_new_delivery, NULL);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_font);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_12_normal));
  lv_label_set_text(label, "Encomenda");
  lv_obj_align(label, btn_new_delivery, LV_ALIGN_CENTER, 0, 10);

  /**************************
   *  Delivery mode button  *
   **************************/

  if (!ui_controller->is_admin && !ui_controller->is_master) {
    lv_obj_align(btn_new_delivery,      NULL, LV_ALIGN_CENTER, -(lv_disp_get_hor_res(NULL) * 7 / 32), -26);
    lv_obj_align(btn_manage_deliveries, NULL, LV_ALIGN_CENTER,   lv_disp_get_hor_res(NULL) * 7 / 32,  -26);
  } 
  else {
    lv_obj_t *btn_delivery_mode = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(btn_delivery_mode, order_tab_order_mode_handler);
    lv_obj_add_style(btn_delivery_mode, LV_BTN_PART_MAIN, &(ui_controller->styles.white_button));
    lv_obj_set_size(btn_delivery_mode, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT * 2 + 10);
    lv_obj_align(btn_delivery_mode, btn_manage_deliveries, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
    lv_btn_set_layout(btn_delivery_mode, LV_LAYOUT_OFF);

    label = lv_label_create(btn_delivery_mode, NULL);
    lv_label_set_text(label, "Modo");
    lv_obj_align(label, btn_delivery_mode, LV_ALIGN_CENTER, 0, -10);
    // lv_obj_set_pos(label, 0, 40);

    label = lv_label_create(btn_delivery_mode, NULL);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_font);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_12_normal));
    lv_label_set_text(label, "Encomenda");
    lv_obj_align(label, btn_delivery_mode, LV_ALIGN_CENTER, 0, 10);
  }
  /****************
   *  Cog Button  *
   ****************/
  if (ui_controller->is_master) {
    lv_obj_t *img_btn_cog = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(img_btn_cog, order_tab_settings_handler);
    lv_btn_set_fit(img_btn_cog, LV_FIT_TIGHT);
    lv_obj_set_style_local_pad_all(img_btn_cog,     LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 10);
    lv_obj_set_style_local_radius(img_btn_cog,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 0);
    lv_obj_set_style_local_border_opa(img_btn_cog,  LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_0);
    lv_obj_set_style_local_bg_opa(img_btn_cog,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_100);
    lv_obj_set_style_local_bg_color(img_btn_cog,    LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
    lv_obj_set_style_local_outline_opa(img_btn_cog, LV_BTN_PART_MAIN, LV_STATE_FOCUSED,      LV_OPA_0);
    lv_obj_set_style_local_bg_color(img_btn_cog,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
    lv_obj_align(img_btn_cog, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -5);

    lv_obj_t *img_btn_cog_img = lv_img_create(img_btn_cog, NULL);
    lv_img_set_src(img_btn_cog_img, &icon_cog);
  }

  /*****************
   * Logout button  *
   *****************/
  lv_obj_t *img_btn_logout = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(img_btn_logout, main_logout_handler);
  lv_btn_set_fit(img_btn_logout, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_all(img_btn_logout,     LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 10);
  lv_obj_set_style_local_radius(img_btn_logout,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 0);
  lv_obj_set_style_local_border_opa(img_btn_logout,  LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_0);
  lv_obj_set_style_local_bg_opa(img_btn_logout,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_100);
  lv_obj_set_style_local_bg_color(img_btn_logout,    LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  lv_obj_set_style_local_outline_opa(img_btn_logout, LV_BTN_PART_MAIN, LV_STATE_FOCUSED,      LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_logout,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_logout, NULL, LV_ALIGN_IN_BOTTOM_MID, 100, -5);

  lv_obj_t *img_btn_logout_img = lv_img_create(img_btn_logout, NULL);
  lv_img_set_src(img_btn_logout_img, &icon_logout);

  /*****************
   * *  Info Button  *
   *****************/
  lv_obj_t *img_btn_info = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(img_btn_info, order_tab_information_handler);
  lv_btn_set_fit(img_btn_info, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_all(img_btn_info,     LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 10);
  lv_obj_set_style_local_radius(img_btn_info,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 0);
  lv_obj_set_style_local_border_opa(img_btn_info,  LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_0);
  lv_obj_set_style_local_bg_opa(img_btn_info,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_100);
  lv_obj_set_style_local_bg_color(img_btn_info,    LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  lv_obj_set_style_local_outline_opa(img_btn_info, LV_BTN_PART_MAIN, LV_STATE_FOCUSED,      LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_info,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_info, NULL, LV_ALIGN_IN_BOTTOM_MID, -100, -5);

  lv_obj_t *img_btn_info_img = lv_img_create(img_btn_info, NULL);
  lv_img_set_src(img_btn_info_img, &icon_info);
}


static void main_create_stock_view(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  /*********
   * BOTTLE
   *********/
  lv_obj_t *cont_bottle = lv_cont_create(parent, NULL);
  lv_cont_set_fit(cont_bottle, LV_FIT_NONE);
  lv_obj_set_size(cont_bottle, 100, 100);
  lv_obj_align(cont_bottle, NULL, LV_ALIGN_CENTER, 0, -30);

  lv_obj_t *img_bottle = lv_img_create(cont_bottle, NULL);
  lv_img_set_src(img_bottle, &icon_bottle_full_inv);
  lv_obj_align(img_bottle, NULL, LV_ALIGN_CENTER, 0, 0);

  /************************
   *   PLUS/MINUS BUTTONS
   ************************/

  lv_obj_t *btn_plus = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_plus, stock_tab_add_bottle_handler);
  lv_obj_add_style(btn_plus, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_plus, 40, 40);
  // lv_obj_align(btn_plus, cont_bottle, LV_ALIGN_OUT_LEFT_MID, -20, -30);
  lv_obj_align(btn_plus, cont_bottle, LV_ALIGN_OUT_RIGHT_MID, 30, -30);

  lv_obj_t *label = lv_label_create(btn_plus, NULL);
  lv_label_set_text(label, LV_SYMBOL_PLUS);

  lv_obj_t *btn_minus = lv_btn_create(parent, btn_plus);
  lv_obj_set_event_cb(btn_minus, stock_tab_remove_bottle_handler);
  lv_obj_align(btn_minus, btn_plus, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

  label = lv_label_create(btn_minus, NULL);
  lv_label_set_text(label, LV_SYMBOL_MINUS);

  /**************
   *  OK BUTTON
   **************/
  lv_obj_t *btn_ok = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_ok, smart_ring_ui_check_handler);
  lv_obj_add_style(btn_ok, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_ok, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_ok, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -18);

  label = lv_label_create(btn_ok, NULL);
  lv_label_set_text(label, "OK");

  lv_obj_t *cont_stock = lv_cont_create(parent, NULL);
  lv_cont_set_fit(cont_stock, LV_FIT_NONE);
  lv_obj_set_size(cont_stock, 80, 40);
  lv_obj_align(cont_stock, cont_bottle, LV_ALIGN_OUT_LEFT_MID, -14, 6);
  // lv_obj_align(cont_stock, cont_bottle, LV_ALIGN_OUT_RIGHT_MID, 14, 6);

  label_stock = lv_label_create(cont_stock, NULL);
  lv_label_set_text_fmt(label_stock, "%d", ui_controller->stock);
  lv_obj_align(label_stock, NULL, LV_ALIGN_CENTER, 0, 0);

  static lv_style_t style_stock_text;
  lv_style_init(&style_stock_text);
  lv_style_set_text_color(&style_stock_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x80, 0x80));

  label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_stock_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_12_normal));
  lv_label_set_text(label, "Stock Atual");
  lv_obj_align(label, cont_stock, LV_ALIGN_OUT_TOP_MID, 0, -4);
}


static void main_create_support_view(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  /******************
   * Options Buttons
   ******************/
  lv_obj_t *btn_support_option_1 = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_support_option_1, main_support_view_option_select_handler);
  lv_obj_add_style(btn_support_option_1, LV_OBJ_PART_MAIN, &(ui_controller->styles.white_button));
  lv_obj_add_style(btn_support_option_1, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_obj_set_width(btn_support_option_1, 150);
  lv_obj_align(btn_support_option_1, NULL, LV_ALIGN_CENTER, 0, -40);

  // Calculate the number of pages based on the number of options available
  current_page  = 1;
  support_pages = (num_options % 2 == 0 ? num_options / 2 : (num_options + 1) / 2);
#ifndef NDEBUG
  printf("Number of support options: %d\nExpected number os pages: "
         "%.2f\nNumber of support pages: %d\n",
         num_options, (float)num_options / 2, support_pages);
#endif
  label_support_option_1 = lv_label_create(btn_support_option_1, NULL);
  lv_label_set_text(label_support_option_1, support_options[0].text);
  lv_obj_set_user_data(btn_support_option_1, &(support_options[0]));
  lv_label_set_long_mode(label_support_option_1, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label_support_option_1, 170);
  lv_label_set_align(label_support_option_1, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_style_local_pad_all(label_support_option_1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);

  if (num_options > 1) {
    lv_obj_t *btn_support_option_2 = lv_btn_create(parent, btn_support_option_1);
    lv_obj_set_event_cb(btn_support_option_2, main_support_view_option_select_handler);
    lv_obj_align(btn_support_option_2, NULL, LV_ALIGN_CENTER, 0, 40);

    label_support_option_2 = lv_label_create(btn_support_option_2, NULL);
    lv_label_set_text(label_support_option_2, support_options[1].text);
    lv_label_set_long_mode(label_support_option_2, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label_support_option_2, 170);
    lv_label_set_align(label_support_option_2, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_style_local_pad_all(label_support_option_2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
#ifndef NDEBUG
    printf("%d\n", support_options[1].type);
#endif
    lv_obj_set_user_data(btn_support_option_2, &(support_options[1]));
  }

  /******************
   * Scroll buttons
   ******************/
  lv_obj_t *button_left = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(button_left, main_support_view_previous_page_handler);
  lv_obj_add_style(button_left, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(button_left, 45, 45);
  lv_obj_align(button_left, NULL, LV_ALIGN_IN_LEFT_MID, 15, 0);

  lv_obj_t *label = lv_label_create(button_left, NULL);
  lv_label_set_text(label, LV_SYMBOL_LEFT);
  lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_22);

  lv_obj_t *button_right = lv_btn_create(parent, button_left);
  lv_obj_set_event_cb(button_right, main_support_view_next_page_handler);
  lv_obj_align(button_right, NULL, LV_ALIGN_IN_RIGHT_MID, -15, 0);

  label = lv_label_create(button_right, NULL);
  lv_label_set_text(label, LV_SYMBOL_RIGHT);
  lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_22);
}


static struct smart_ring_ui_debug_sensor_t {
  lv_obj_t *label_sensor_new;
  lv_obj_t *label_sensor_old;
  lv_obj_t *label_stability;
  lv_obj_t *img_stability;
} container_debug_sensor;


void smart_ring_main_debug_update_sensor_values(int old_value, int new_value, bool stable) {
  if (old_value != NO_UPDATE) {
    lv_label_set_text_fmt(container_debug_sensor.label_sensor_old, "%d", old_value);
  }
  if (new_value != NO_UPDATE) {
    lv_label_set_text_fmt(container_debug_sensor.label_sensor_new, "%d", new_value);
  }
  if (stable) {
    lv_img_set_src(container_debug_sensor.img_stability, &icon_stable);
    lv_label_set_text(container_debug_sensor.label_stability, "#00FF00 Estável");
  } else {
    lv_img_set_src(container_debug_sensor.img_stability, &icon_unstable);
    lv_label_set_text(container_debug_sensor.label_stability, "#FF0000 Instável");
  }
}


static void smart_ring_ui_debug_calibrate_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_29);
  }
}


void main_create_debug_menu(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Debug", ui_controller);

  static lv_style_t style_cont;
  lv_style_init(&style_cont);
  lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 1);
  lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT,  LV_COLOR_MAKE(0x97, 0x97, 0x97));
  lv_style_set_radius(&style_cont, LV_STATE_DEFAULT, 8);

  lv_obj_t *cont_debug = lv_cont_create(parent, NULL);
  lv_obj_add_style(cont_debug, LV_OBJ_PART_MAIN, &style_cont);
  lv_obj_set_size(cont_debug, 300, 150);
  lv_obj_align(cont_debug, NULL, LV_ALIGN_CENTER, 0, -10);

  /*****************
   * Sensor values
   *****************/
  lv_obj_t *img_sensor_arrow = lv_img_create(cont_debug, NULL);
  lv_img_set_src(img_sensor_arrow, &icon_down_arrow);
  lv_obj_align(img_sensor_arrow, NULL, LV_ALIGN_CENTER, -100, 10);

  container_debug_sensor.label_sensor_old = lv_label_create(cont_debug, NULL);
  lv_label_set_text_fmt(container_debug_sensor.label_sensor_old, "0");
  lv_obj_add_style(container_debug_sensor.label_sensor_old, LV_LABEL_PART_MAIN, &ui_controller->styles.font_18_bold);
  lv_obj_set_auto_realign(container_debug_sensor.label_sensor_old, true);
  lv_obj_align_origo(container_debug_sensor.label_sensor_old, img_sensor_arrow,LV_ALIGN_IN_TOP_MID, 0, -17);

  lv_obj_t *label_sensor_type = lv_label_create(cont_debug, NULL);
  lv_label_set_text(label_sensor_type, "Novo");
  lv_obj_set_style_local_text_font(label_sensor_type, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);
  lv_obj_align(label_sensor_type, container_debug_sensor.label_sensor_old, LV_ALIGN_IN_TOP_MID, 0, -16);

  label_sensor_type = lv_label_create(cont_debug, NULL);
  lv_label_set_text(label_sensor_type, "Antigo");
  lv_obj_set_style_local_text_font(label_sensor_type, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);
  lv_obj_align(label_sensor_type, img_sensor_arrow, LV_ALIGN_IN_BOTTOM_MID, 0, 25);

  container_debug_sensor.label_sensor_new = lv_label_create(cont_debug, NULL);
  lv_label_set_text(container_debug_sensor.label_sensor_new, "0");
  lv_obj_add_style(container_debug_sensor.label_sensor_new, LV_LABEL_PART_MAIN, &ui_controller->styles.font_18_bold);
  lv_obj_set_auto_realign(container_debug_sensor.label_sensor_new, true);
  lv_obj_align_origo(container_debug_sensor.label_sensor_new, label_sensor_type, LV_ALIGN_IN_BOTTOM_MID, 0, 10);

  /*****************
   *  Calibrate button
   *****************/
  lv_obj_t *btn_calibrate = lv_btn_create(cont_debug, NULL);
  lv_obj_set_event_cb(btn_calibrate, smart_ring_ui_debug_calibrate_handler);
  lv_obj_add_style(btn_calibrate, LV_BTN_PART_MAIN, &(ui_controller->styles.white_button));
  lv_obj_set_size(btn_calibrate, SR_DEFAULT_BTN_WIDTH, 66);
  lv_obj_align(btn_calibrate, NULL, LV_ALIGN_CENTER, 0, 8);

  lv_obj_t *label = lv_label_create(btn_calibrate, NULL);
  lv_label_set_text(label, "Calibrar");

  /********************
   * FIRMWARE VERSION *
   ********************/
  lv_obj_t *label_fw_v = lv_label_create(cont_debug, NULL);
  lv_label_set_recolor(label_fw_v, true);
  lv_label_set_text_fmt(label_fw_v, "#979797 Firmware: %s#", ui_controller->firmware_version);
  lv_obj_align(label_fw_v, btn_calibrate, LV_ALIGN_OUT_TOP_MID, 0, -25);

  /***************
   *  Stability  *
   ***************/
  container_debug_sensor.label_stability = lv_label_create(parent, NULL);
  lv_label_set_recolor(container_debug_sensor.label_stability, true);
  lv_label_set_text(container_debug_sensor.label_stability, "#FF0000 Instável");
  lv_obj_add_style(container_debug_sensor.label_stability, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_obj_set_auto_realign(container_debug_sensor.label_stability, true);
  lv_obj_align_origo(container_debug_sensor.label_stability, NULL, LV_ALIGN_IN_RIGHT_MID, -60, -30);

  container_debug_sensor.img_stability = lv_img_create(parent, NULL);
  lv_img_set_src(container_debug_sensor.img_stability, &icon_unstable);
  lv_obj_set_auto_realign(container_debug_sensor.img_stability, true);
  lv_obj_align_origo(container_debug_sensor.img_stability, container_debug_sensor.label_stability, LV_ALIGN_IN_BOTTOM_MID, 0, 28);

  /*******************
   *  Restart button *
   *******************/
  lv_obj_t *btn_restart = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_restart, smart_ring_ui_debug_restart_handler);
  lv_obj_add_style(btn_restart, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_restart, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_restart, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 20, -12);

  label = lv_label_create(btn_restart, NULL);
  lv_label_set_text(label, "Reiniciar");

  /*******************
   *  Update button  *
   *******************/
  lv_obj_t *btn_update = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_update, smart_ring_ui_debug_update_handler);
  lv_obj_add_style(btn_update, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_update, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_update, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -12);

  label = lv_label_create(btn_update, NULL);
  lv_label_set_text(label, "Atualizar");

  /****************
   *  Home button *
   ****************/
  lv_obj_t *img_btn_home = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(img_btn_home, main_home_handler);
  lv_btn_set_fit(img_btn_home, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_all(img_btn_home,     LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 10);
  lv_obj_set_style_local_radius(img_btn_home,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 0);
  lv_obj_set_style_local_border_opa(img_btn_home,  LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_0);
  lv_obj_set_style_local_bg_opa(img_btn_home,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_home,    LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  lv_obj_set_style_local_outline_opa(img_btn_home, LV_BTN_PART_MAIN, LV_STATE_FOCUSED, LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_home,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_home, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -1);

  lv_obj_t *img_btn_home_img = lv_img_create(img_btn_home, NULL);
  lv_img_set_src(img_btn_home_img, &icon_home);

  smart_ring_main_debug_update_sensor_values(NO_UPDATE, NO_UPDATE, false);
}


void main_create_reset_menu(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Reiniciar", ui_controller);

  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  /*************************
   *  Restart Device button
   *************************/
  lv_obj_t *btn_device_reset = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_device_reset, smart_ring_ui_restart_device_handler);
  lv_obj_add_style(btn_device_reset, LV_BTN_PART_MAIN, &(ui_controller->styles.white_button));
  lv_obj_set_size(btn_device_reset, 125, 100);
  lv_obj_align(btn_device_reset, NULL, LV_ALIGN_CENTER, -75, 0);
  lv_btn_set_layout(btn_device_reset, LV_LAYOUT_OFF);

  lv_obj_t *label = lv_label_create(btn_device_reset, NULL);
  lv_label_set_text(label, "Reiniciar");
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_obj_align(label, btn_device_reset, LV_ALIGN_CENTER, 0, -10);
  // lv_obj_set_pos(label, 0, 40);

  label = lv_label_create(btn_device_reset, NULL);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_text);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_label_set_text(label, "Dispositivo");
  lv_obj_align(label, btn_device_reset, LV_ALIGN_CENTER, 0, 10);

  /*************************
   *  Restart WiFi button
   *************************/
  lv_obj_t *btn_wifi_reset = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_wifi_reset, smart_ring_ui_restart_wifi_handler);
  lv_obj_add_style(btn_wifi_reset, LV_BTN_PART_MAIN, &(ui_controller->styles.white_button));
  lv_obj_set_size(btn_wifi_reset, 125, 100);
  lv_obj_align(btn_wifi_reset, NULL, LV_ALIGN_CENTER, 75, 0);
  lv_btn_set_layout(btn_wifi_reset, LV_LAYOUT_OFF);

  label = lv_label_create(btn_wifi_reset, NULL);
  lv_label_set_text(label, "Reiniciar");
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_obj_align(label, btn_wifi_reset, LV_ALIGN_CENTER, 0, -10);
  // lv_obj_set_pos(label, 0, 40);

  label = lv_label_create(btn_wifi_reset, NULL);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_text);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_label_set_text(label, "WiFi");
  lv_obj_align(label, btn_wifi_reset, LV_ALIGN_CENTER, 0, 10);

  /****************
   *  Home button
   ****************/
  lv_obj_t *img_btn_home = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(img_btn_home, main_home_handler);
  lv_btn_set_fit(img_btn_home, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_all(img_btn_home,     LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 10);
  lv_obj_set_style_local_radius(img_btn_home,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 0);
  lv_obj_set_style_local_border_opa(img_btn_home,  LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_0);
  lv_obj_set_style_local_bg_opa(img_btn_home,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_100);
  lv_obj_set_style_local_bg_color(img_btn_home,    LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  lv_obj_set_style_local_outline_opa(img_btn_home, LV_BTN_PART_MAIN, LV_STATE_FOCUSED, LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_home,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_home, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -5);

  lv_obj_t *img_btn_home_img = lv_img_create(img_btn_home, NULL);
  lv_img_set_src(img_btn_home_img, &icon_home);
}


void main_create_info_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

#ifndef NDEBUG
  ESP_LOGI(TAG, "Creating header");
#endif
  create_header(parent, "Informação do dispositivo", ui_controller);

  /* Container */
  static lv_style_t style_cont;
  lv_style_init(&style_cont);
  lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 1);
  lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x97, 0x97, 0x97));
  lv_style_set_radius(&style_cont, LV_STATE_DEFAULT, 8);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Creating container");
#endif
  lv_obj_t *cont_info = lv_cont_create(parent, NULL);
  lv_obj_add_style(cont_info, LV_OBJ_PART_MAIN, &style_cont);
  lv_obj_set_size(cont_info, 150, 170);
  lv_obj_align(cont_info, NULL, LV_ALIGN_CENTER, 0, 15);

  /* Labels */
  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
#ifndef NDEBUG
  ESP_LOGI(TAG, "Creating label 1");
#endif
  lv_obj_t *label = lv_label_create(cont_info, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_blue_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_text(label, "Nº de série:");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -64);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Adding mac address");
#endif
  lv_obj_t *label_serial_number = lv_label_create(cont_info, NULL);
  lv_label_set_text(label_serial_number, ui_controller->mac_address);
  lv_obj_align(label_serial_number, label, LV_ALIGN_CENTER, 0, 24);
  lv_label_set_long_mode(label_serial_number, LV_LABEL_LONG_SROLL_CIRC);

  label = lv_label_create(cont_info, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_blue_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_text(label, "Ligado a:");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -10);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Adding ssid");
#endif
  lv_obj_t *label_ssid = lv_label_create(cont_info, NULL);
  if (ui_controller->connection_type == 'w')
    lv_label_set_text(label_ssid, ui_controller->ssid);
  else
    lv_label_set_text(label_ssid, "-"); /** TODO: add GSM and LoRA */
  lv_label_set_long_mode(label_ssid, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_set_width(label_ssid, 150);
  lv_label_set_align(label_ssid, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_auto_realign(label_ssid, true);
  lv_obj_align_origo(label_ssid, label, LV_ALIGN_CENTER, 0, 24);

  label = lv_label_create(cont_info, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_blue_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN,
                   &(ui_controller->styles.font_16_normal));
  lv_label_set_text(label, "Ligação:");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 44);
#ifndef NDEBUG
  ESP_LOGI(TAG, "Adding communication type %c", ui_controller->connection_type);
#endif
  lv_obj_t *label_connection_type = lv_label_create(cont_info, NULL);
  switch (ui_controller->connection_type) {
  case 'w':
    lv_label_set_text(label_connection_type, "WiFi");
    break;
  case 'g':
    lv_label_set_text(label_connection_type, "GSM");
    break;
  case 'l':
    lv_label_set_text(label_connection_type, "Lora");
    break;
  default:
    lv_label_set_text(label_connection_type, "Error");
    break;
  }
  lv_obj_align(label_connection_type, label, LV_ALIGN_CENTER, 0, 24);

  /****************
   *  Home button
   ****************/
  lv_obj_t *img_btn_home = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(img_btn_home, main_home_handler);
  lv_btn_set_fit(img_btn_home, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_all(img_btn_home, LV_BTN_PART_MAIN,
                                 LV_BTN_STATE_RELEASED, 10);
  lv_obj_set_style_local_radius(img_btn_home, LV_BTN_PART_MAIN,
                                LV_BTN_STATE_RELEASED, 0);
  lv_obj_set_style_local_border_opa(img_btn_home, LV_BTN_PART_MAIN,
                                    LV_BTN_STATE_RELEASED, LV_OPA_0);
  lv_obj_set_style_local_bg_opa(img_btn_home, LV_BTN_PART_MAIN,
                                LV_BTN_STATE_RELEASED, LV_OPA_100);
  lv_obj_set_style_local_bg_color(img_btn_home, LV_BTN_PART_MAIN,
                                  LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  lv_obj_set_style_local_outline_opa(img_btn_home, LV_BTN_PART_MAIN,
                                     LV_STATE_FOCUSED, LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_home, LV_BTN_PART_MAIN,
                                  LV_STATE_PRESSED | LV_STATE_FOCUSED,
                                  LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_home, NULL, LV_ALIGN_IN_BOTTOM_MID, -115, -5);

  lv_obj_t *img_btn_home_img = lv_img_create(img_btn_home, NULL);
  lv_img_set_src(img_btn_home_img, &icon_home);
}

void main_create_dashboard_menu(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  /************
   * Style
   *************/

#ifndef NDEBUG
  ESP_LOGI(TAG, "Options menu style creation");
#endif
  /* No padding or margins style */
  static lv_style_t style_options_menu;
  lv_style_init(&style_options_menu);
  lv_style_set_pad_inner(&style_options_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_pad_left(&style_options_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_pad_right(&style_options_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_margin_left(&style_options_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_margin_right(&style_options_menu, LV_STATE_DEFAULT, 0);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Background style creation");
#endif
  /* background style */
  static lv_style_t style_tv_bg;
  lv_style_init(&style_tv_bg);
  lv_style_set_bg_color(&style_tv_bg, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xEB, 0xED, 0xF0));
  lv_style_set_pad_top(&style_tv_bg, LV_STATE_DEFAULT, 0);
  lv_style_set_pad_bottom(&style_tv_bg, LV_STATE_DEFAULT, 0);
  lv_style_set_shadow_width(&style_tv_bg, LV_STATE_DEFAULT, 14);
  lv_style_set_shadow_color(&style_tv_bg, LV_STATE_DEFAULT, LV_COLOR_SILVER);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Button released style creation");
#endif

  /* button style released */
  static lv_style_t style_tv_btn;
  lv_style_init(&style_tv_btn);
  lv_style_set_bg_color(&style_tv_btn, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_text_color(&style_tv_btn, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x97, 0x97, 0x97));
  lv_style_set_pad_ver(&style_tv_btn, LV_STATE_DEFAULT, 15);
  lv_style_set_bg_opa(&style_tv_btn, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_style_set_radius(&style_tv_btn, LV_STATE_DEFAULT, SR_DEFAULT_BTN_RADIUS);
  lv_style_set_border_width(&style_tv_btn, LV_STATE_DEFAULT, 0);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Button pressed style creation");
#endif

  /* button style pressed */
  lv_style_set_bg_color(&style_tv_btn, LV_STATE_PRESSED, LV_COLOR_WHITE);
  lv_style_set_border_width(&style_tv_btn, LV_STATE_PRESSED, 0);
  lv_style_set_bg_opa(&style_tv_btn, LV_STATE_PRESSED, LV_OPA_TRANSP);
  lv_style_set_radius(&style_tv_btn, LV_STATE_PRESSED, 0);
  lv_style_set_text_color(&style_tv_btn, LV_STATE_PRESSED, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_text_color(&style_tv_btn, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_style_set_text_color(&style_tv_btn, LV_STATE_CHECKED, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

#ifndef NDEBUG
  ESP_LOGI(TAG, "Indicator style creation");
#endif

  /* indicator style */
  static lv_style_t style_tv_indic;
  lv_style_init(&style_tv_indic);
  lv_style_set_bg_color(&style_tv_indic, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  /***********
   * Tabview *
   ***********/
#ifndef NDEBUG
  ESP_LOGI(TAG, "Tabviewcreation");
#endif
  lv_obj_t *tabview = lv_tabview_create(parent, NULL);
  lv_obj_set_size(tabview, 320, 240);
  lv_obj_add_style(tabview, LV_OBJ_PART_MAIN, &(ui_controller->styles.blank_page));
  lv_obj_add_style(tabview, LV_OBJ_PART_MAIN, &style_options_menu);
  lv_obj_add_style(tabview, LV_TABVIEW_PART_TAB_BG, &style_tv_bg);
  lv_obj_add_style(tabview, LV_TABVIEW_PART_TAB_BTN, &style_tv_btn);
  lv_obj_add_style(tabview, LV_TABVIEW_PART_INDIC, &style_tv_indic);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Add order tab");
#endif
  /********
   * Tabs *
   ********/
  lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Encomendas");

#ifndef NDEBUG
  ESP_LOGI(TAG, "Add stock tab");
#endif
  lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Stock");

#ifndef NDEBUG
  ESP_LOGI(TAG, "Add support tab");
#endif
  lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Suporte");

#ifndef NDEBUG
  ESP_LOGI(TAG, "Add change tab callback");
#endif
  lv_obj_set_event_cb(tabview, main_tabview_screen_change_handler);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Add scroll behaviour");
#endif
  lv_page_set_scrollable_fit(tab1, LV_FIT_PARENT);
  lv_page_set_scrollable_fit(tab2, LV_FIT_PARENT);
  lv_page_set_scrollable_fit(tab3, LV_FIT_PARENT);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Remove scroll propagation");
#endif
  lv_page_set_scroll_propagation(tab1, false);
  lv_page_set_scroll_propagation(tab2, false);
  lv_page_set_scroll_propagation(tab3, false);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Create order view");
#endif
  main_create_order_view(tab1, ui_controller);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Create stock view");
#endif
  main_create_stock_view(tab2, ui_controller);

#ifndef NDEBUG
  ESP_LOGI(TAG, "Create support view");
#endif
  main_create_support_view(tab3, ui_controller);
}

void main_update_rssi_signal(uint8_t rssi_level) {
#ifndef NDEBUG
  ESP_LOGI(TAG, "Updating icon to %d", rssi_level);
#endif
  switch (rssi_level) {
  case 0:  lv_img_set_src(com_signal_img, &wifi_signal_0); break;
  case 1:  lv_img_set_src(com_signal_img, &wifi_signal_1); break;
  case 2:  lv_img_set_src(com_signal_img, &wifi_signal_2); break;
  case 3:  lv_img_set_src(com_signal_img, &wifi_signal_3); break;
  default: lv_img_set_src(com_signal_img, &wifi_signal_0); break;
  }
}

void main_update_device_name(char *device_name) {
  lv_label_set_text(device_name_label, device_name);
}

void main_update_eta_information(char *information) {
  lv_label_set_text(eta_label, information);
}

void main_update_bottle_icon(struct smart_ring_ui_controller_t *ui_controller) {

  uint8_t percentage = map(ui_controller->current_deposit, ui_controller->no_deposit,
                                                           ui_controller->full_deposit, 0, 100);

#ifndef NDEBUG
  ESP_LOGI(TAG, "New UI percentage : %d", percentage);
#endif

  if (percentage >= 75) {
    lv_img_set_src(main_bottle_img, &icon_bottle_100);
  } else if (percentage >= 50) {
    lv_img_set_src(main_bottle_img, &icon_bottle_75);
  } else if (percentage >= 25) {
    lv_img_set_src(main_bottle_img, &icon_bottle_50);
  } else {
    lv_img_set_src(main_bottle_img, &icon_bottle_25);
  }
}

void main_update_stock(int stock) {
  smart_ring_ui_get_controller()->updated_stock = stock;
  lv_label_set_text_fmt(label_stock, "%d", stock);
}

void main_update_order_mode(char order_mode) {

  if (order_mode == 'm') {
    lv_img_set_src(delivery_mode_img, &icon_manual);
  } else {
    lv_img_set_src(delivery_mode_img, &icon_auto);
  }
}

static void smart_ring_ui_sleep_handler(lv_obj_t *element, lv_event_t event) {

  if (event == LV_EVENT_RELEASED) {

    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_5);
    gpio_set_level(CONFIG_LV_DISP_PIN_BCKL, 1);
  }
}

void smart_ring_ui_create_sleep_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  lv_obj_set_event_cb(parent, smart_ring_ui_sleep_handler);
}

