/**
 * @file order.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Responsible for the oprder screens
 * @version 2.0
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2023
 *
 * 

_____________________________________/\\\___________________________________________________        
 ____________________________________\/\\\___________________________________________________       
  ____________________________________\/\\\___________________________________________________      
   _____/\\\\\_____/\\/\\\\\\\_________\/\\\______/\\\\\\\\___/\\/\\\\\\\____________/\\\\\\\\_     
    ___/\\\///\\\__\/\\\/////\\\___/\\\\\\\\\____/\\\/////\\\_\/\\\/////\\\_________/\\\//////__    
     __/\\\__\//\\\_\/\\\___\///___/\\\////\\\___/\\\\\\\\\\\__\/\\\___\///_________/\\\_________   
      _\//\\\__/\\\__\/\\\_________\/\\\__\/\\\__\//\\///////___\/\\\_______________\//\\\________  
       __\///\\\\\/___\/\\\_________\//\\\\\\\/\\__\//\\\\\\\\\\_\/\\\__________/\\\__\///\\\\\\\\_ 
        ____\/////_____\///___________\///////\//____\//////////__\///__________\///_____\////////__

 */

#include "order.h"

// Tag for logging to the monitor
static const char *TAG = "UI_ORDER";

LV_IMG_DECLARE(icon_bottle_full_inv)
LV_IMG_DECLARE(icon_logout)
LV_IMG_DECLARE(icon_home)
LV_IMG_DECLARE(icon_cup)
LV_IMG_DECLARE(icon_truck)
LV_IMG_DECLARE(icon_alert)
LV_IMG_DECLARE(icon_delivery_ok)
LV_IMG_DECLARE(icon_delivery_error)

// Delivery table variables and functions

int page = 1, number_of_pages;
lv_obj_t *table_deliveries;

static void order_manage_next_page_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {

    smart_ring_ui_get_controller()->standby_controller.has_callback = true;

#ifndef NDEBUG
    ESP_LOGI(TAG, "Next page : %d - %d", page, page + 1);
#endif

    if (page == number_of_pages ||
        smart_ring_ui_get_controller()->deliveries == NULL) {
      return;
    }

    page++;

    for (size_t i = 0; i < 3; i++) {
      lv_table_set_cell_value(table_deliveries, i + 1, 0, " ");
      lv_table_set_cell_value(table_deliveries, i + 1, 1, " ");
      lv_table_set_cell_value(table_deliveries, i + 1, 2, " ");
      lv_table_set_cell_value(table_deliveries, i + 1, 3, " ");
    }

    for (size_t i = page * 3 - 3;
         i < page * 3 &&
         i < smart_ring_ui_get_controller()->number_of_deliveries;
         i++) {
#ifndef NDEBUG
      ESP_LOGI(TAG, "Delivery #%d", i + 1);
#endif
      lv_table_set_cell_value_fmt(table_deliveries, (i - (page * 3 - 3)) + 1, 0, "%s", smart_ring_ui_get_controller()->deliveries[i].date);
      lv_table_set_cell_value_fmt(table_deliveries, (i - (page * 3 - 3)) + 1, 1, "%d", smart_ring_ui_get_controller()->deliveries[i].bottles);
      lv_table_set_cell_value_fmt(table_deliveries, (i - (page * 3 - 3)) + 1, 2, "%d", smart_ring_ui_get_controller()->deliveries[i].cups);
      lv_table_set_cell_value_fmt(table_deliveries, (i - (page * 3 - 3)) + 1, 3, "%s", smart_ring_ui_get_controller()->deliveries[i].status);
    }
  }
}

static void order_manage_previous_page_handler(lv_obj_t *button,
                                               lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {

    smart_ring_ui_get_controller()->standby_controller.has_callback = true;

#ifndef NDEBUG
    ESP_LOGI(TAG, "Previous page : %d - %d", page, page - 1);
#endif

    if (page == 1 || smart_ring_ui_get_controller()->deliveries == NULL) {
      return;
    }

    page--;

    for (size_t i = 0; i < 3; i++) {
      lv_table_set_cell_value(table_deliveries, i + 1, 0, " ");
      lv_table_set_cell_value(table_deliveries, i + 1, 1, " ");
      lv_table_set_cell_value(table_deliveries, i + 1, 2, " ");
      lv_table_set_cell_value(table_deliveries, i + 1, 3, " ");
    }

    for (size_t i = page * 3 - 3;
         i < page * 3 &&
         i < smart_ring_ui_get_controller()->number_of_deliveries;
         i++) {
      lv_table_set_cell_value_fmt(table_deliveries, (i - (page * 3 - 3)) + 1, 0, "%s", smart_ring_ui_get_controller()->deliveries[i].date);
      lv_table_set_cell_value_fmt(table_deliveries, (i - (page * 3 - 3)) + 1, 1, "%d", smart_ring_ui_get_controller()->deliveries[i].bottles);
      lv_table_set_cell_value_fmt(table_deliveries, (i - (page * 3 - 3)) + 1, 2, "%d", smart_ring_ui_get_controller()->deliveries[i].cups);
      lv_table_set_cell_value_fmt(table_deliveries, (i - (page * 3 - 3)) + 1, 3, "%s", smart_ring_ui_get_controller()->deliveries[i].status);
    }
  }
}

static void order_change_mode_handler(lv_obj_t *toggle, lv_event_t event) {

  if (event == LV_EVENT_RELEASED) {

    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
#ifndef NDEBUG
    ESP_LOGI(TAG, "Toggle state : %d", lv_switch_get_state(toggle));
#endif

    modal_create_template(lv_scr_act(), smart_ring_ui_get_controller(),
                          "Modo de encomenda", "A comunicar", true);

    if (lv_switch_get_state(toggle)) {
      smart_ring_ui_get_controller()->updated_order_mode = 'a';
      lv_switch_off(toggle, LV_ANIM_OFF);
    } else {
      smart_ring_ui_get_controller()->updated_order_mode = 'm';
      lv_switch_on(toggle, LV_ANIM_OFF);
    }

    smart_ring_ui_get_controller()->flags.flag.change_order_mode = true;
  }
}

static void smart_ring_ui_order_select_bottles_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_get_controller()->order.changing_bottles = true;
  }
}

static void smart_ring_ui_order_select_cups_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_get_controller()->order.changing_bottles = false;
  }
}

lv_obj_t *label_bottle_quantity;
lv_obj_t *label_cups_quantity;
static void smart_ring_ui_order_add_item_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;

    if (smart_ring_ui_get_controller()->order.changing_bottles) {
      lv_label_set_text_fmt(label_bottle_quantity, "%d", ++smart_ring_ui_get_controller()->order.bottles);
    } else {
      lv_label_set_text_fmt(label_cups_quantity,   "%d", ++smart_ring_ui_get_controller()->order.cups);
    }
  }
}

static void smart_ring_ui_order_remove_item_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;

    struct smart_ring_ui_controller_t *ui_controller = smart_ring_ui_get_controller();

    if (smart_ring_ui_get_controller()->order.changing_bottles) {
      ui_controller->order.bottles > 0 ? --ui_controller->order.bottles : NULL;
      lv_label_set_text_fmt(label_bottle_quantity, "%d", ui_controller->order.bottles);
    } else {
      ui_controller->order.cups > 0 ? --ui_controller->order.cups : NULL;
      lv_label_set_text_fmt(label_cups_quantity, "%d", ui_controller->order.cups);
    }
  }
}

static void smart_ring_ui_order_start_order_handler(lv_obj_t *button, lv_event_t event) {

  if (event == LV_EVENT_RELEASED) {

    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    if (smart_ring_ui_get_controller()->order.bottles != 0 ||
        smart_ring_ui_get_controller()->order.cups != 0) {
      smart_ring_ui_get_controller()->flags.flag.start_order = true;
    }
  }
}

void order_create_new_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  create_header(parent, "Nova encomenda", ui_controller);

  /**********
   * BOTTLE
   **********/
  lv_obj_t *cont_bottle = lv_cont_create(parent, NULL);
  lv_obj_set_event_cb(cont_bottle, smart_ring_ui_order_select_bottles_handler);
  lv_cont_set_fit(cont_bottle, LV_FIT_NONE);
  lv_obj_set_size(cont_bottle, 80, 90);
  lv_obj_align(cont_bottle, NULL, LV_ALIGN_CENTER, -76, -16);

  lv_obj_t *img_bottle = lv_img_create(cont_bottle, NULL);
  lv_img_set_src(img_bottle, &icon_bottle_full_inv);
  lv_obj_align(img_bottle, NULL, LV_ALIGN_CENTER, 0, 0);

  static lv_style_t style_gray_text;
  lv_style_init(&style_gray_text);
  lv_style_set_text_color(&style_gray_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x80, 0x80));

  lv_obj_t *label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_gray_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_label_set_text(label, "Garrafões");
  lv_obj_align(label, cont_bottle, LV_ALIGN_OUT_TOP_MID, 0, -4);

  /* BOTTLES QUANTITY */
  lv_obj_t *cont_bottle_quantity = lv_cont_create(parent, NULL);
  lv_cont_set_fit(cont_bottle_quantity, LV_FIT_NONE);
  lv_obj_set_size(cont_bottle_quantity, 28, 40);
  lv_obj_align(cont_bottle_quantity, cont_bottle, LV_ALIGN_IN_LEFT_MID, -14, 0);

  label_bottle_quantity = lv_label_create(cont_bottle_quantity, NULL);
  lv_label_set_text_fmt(label_bottle_quantity, "0");
  lv_obj_set_auto_realign(label_bottle_quantity, true);
  lv_obj_align_origo(label_bottle_quantity, NULL, LV_ALIGN_CENTER, 0, 0);

  /*********
   *  CUP
   *********/
  lv_obj_t *cont_cup = lv_cont_create(parent, NULL);
  lv_obj_set_event_cb(cont_cup, smart_ring_ui_order_select_cups_handler);
  lv_cont_set_fit(cont_cup, LV_FIT_NONE);
  lv_obj_set_size(cont_cup, 80, 90);
  lv_obj_align(cont_cup, cont_bottle, LV_ALIGN_OUT_RIGHT_MID, 16, 0);

  lv_obj_t *img_cup = lv_img_create(cont_cup, NULL);
  lv_img_set_src(img_cup, &icon_cup);
  lv_obj_align(img_cup, NULL, LV_ALIGN_CENTER, 0, 0);

  label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_gray_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_label_set_text(label, "Copos");
  lv_obj_align(label, cont_cup, LV_ALIGN_OUT_TOP_MID, 0, -4);

  label = lv_label_create(cont_cup, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &style_gray_text);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_label_set_text(label, "x100");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, -2, 30);

  /* CUP QUANTITY */
  lv_obj_t *cont_cup_quantity = lv_cont_create(parent, NULL);
  lv_cont_set_fit(cont_cup_quantity, LV_FIT_NONE);
  lv_obj_set_size(cont_cup_quantity, 28, 40);
  lv_obj_align(cont_cup_quantity, cont_cup, LV_ALIGN_IN_RIGHT_MID, 14, 0);

  label_cups_quantity = lv_label_create(cont_cup_quantity, NULL);
  lv_label_set_text_fmt(label_cups_quantity, "0");
  lv_obj_set_auto_realign(label_cups_quantity, true);
  lv_obj_align_origo(label_cups_quantity, NULL, LV_ALIGN_CENTER, 0, 0);

  /**********************
   * PLUS/MINUS BUTTONS *
   **********************/
  lv_obj_t *btn_plus = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_plus, smart_ring_ui_order_add_item_handler);
  lv_obj_add_style(btn_plus, LV_BTN_PART_MAIN,
                   &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_plus, 40, 40);
  lv_obj_align(btn_plus, cont_cup, LV_ALIGN_OUT_RIGHT_MID, 26, -30);

  label = lv_label_create(btn_plus, NULL);
  lv_label_set_text(label, LV_SYMBOL_PLUS);

  lv_obj_t *btn_minus = lv_btn_create(parent, btn_plus);
  lv_obj_set_event_cb(btn_minus, smart_ring_ui_order_remove_item_handler);
  lv_obj_align(btn_minus, btn_plus, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

  label = lv_label_create(btn_minus, NULL);
  lv_label_set_text(label, LV_SYMBOL_MINUS);

  /**************
   *  OK BUTTON
   **************/
  lv_obj_t *btn_ok = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_ok, smart_ring_ui_order_start_order_handler);
  lv_obj_add_style(btn_ok, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_ok, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_ok, parent, LV_ALIGN_IN_BOTTOM_MID, 0, -18);

  label = lv_label_create(btn_ok, NULL);
  lv_label_set_text(label, "OK");

  /****************
   *  Back button
   ****************/
  lv_obj_t *img_btn_back = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(img_btn_back, main_logout_handler);
  lv_btn_set_fit(img_btn_back, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_all(img_btn_back,     LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 10);
  lv_obj_set_style_local_radius(img_btn_back,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 0);
  lv_obj_set_style_local_border_opa(img_btn_back,  LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_0);
  lv_obj_set_style_local_bg_opa(img_btn_back,      LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_OPA_100);
  lv_obj_set_style_local_bg_color(img_btn_back,    LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  lv_obj_set_style_local_outline_opa(img_btn_back, LV_BTN_PART_MAIN, LV_STATE_FOCUSED, LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_back,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED,LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_back, NULL, LV_ALIGN_IN_BOTTOM_MID, 100, -5);

  lv_obj_t *img_btn_back_img = lv_img_create(img_btn_back, NULL);
  lv_img_set_src(img_btn_back_img, &icon_logout);

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
  lv_obj_set_style_local_bg_color(img_btn_home,    LV_BTN_PART_MAIN,  LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  lv_obj_set_style_local_outline_opa(img_btn_home, LV_BTN_PART_MAIN, LV_STATE_FOCUSED, LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_home,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_home, NULL, LV_ALIGN_IN_BOTTOM_MID, -100, -5);

  lv_obj_t *img_btn_home_img = lv_img_create(img_btn_home, NULL);
  lv_img_set_src(img_btn_home_img, &icon_home);
}

static void smart_ring_ui_order_cancel_order_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_24);
  }
}

static void smart_ring_ui_order_accept_order_handler(lv_obj_t *button, lv_event_t event) {
  if (event == LV_EVENT_RELEASED) {
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_25);
  }
}

static lv_obj_t *table_checkout;
static lv_obj_t *btn_check_order;
static lv_obj_t *label_check_order;
static lv_obj_t *spinner_check_order;

void smart_ring_ui_order_update_review_data(float price) {

  char string_price[10];
  sprintf(string_price, "%.2f €", price);
  lv_table_set_cell_value(table_checkout, 2, 1, string_price);

  lv_obj_set_style_local_bg_color(btn_check_order,   LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_obj_set_style_local_text_color(btn_check_order, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

  lv_obj_set_click(btn_check_order, true);

  lv_obj_set_hidden(spinner_check_order, true);
  lv_obj_set_hidden(label_check_order, false);
}

void order_create_review_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Nova encomenda", ui_controller);

  /**********
   *  TABLE
   **********/
  static lv_style_t style_cells;
  lv_style_init(&style_cells);
  lv_style_set_border_width(&style_cells, LV_STATE_DEFAULT, 1);
  lv_style_set_pad_top(&style_cells,      LV_STATE_DEFAULT, 6);
  lv_style_set_pad_bottom(&style_cells,   LV_STATE_DEFAULT, 6);
  lv_style_set_pad_right(&style_cells,    LV_STATE_DEFAULT, 0);
  lv_style_set_pad_left(&style_cells,     LV_STATE_DEFAULT, 12);

  static lv_style_t style_cells_2;
  lv_style_init(&style_cells_2);
  lv_style_set_border_width(&style_cells_2, LV_STATE_DEFAULT, 0);

  static lv_style_t style_table;
  lv_style_init(&style_table);
  lv_style_set_border_width(&style_table, LV_STATE_DEFAULT, 1);
  lv_style_set_pad_right(&style_table,    LV_STATE_DEFAULT, 0);
  lv_style_set_pad_left(&style_table,     LV_STATE_DEFAULT, 0);
  lv_style_set_pad_top(&style_table,      LV_STATE_DEFAULT, 0);
  lv_style_set_pad_bottom(&style_table,   LV_STATE_DEFAULT, 0);

  /* Table */
  table_checkout = lv_table_create(parent, NULL);
  lv_obj_add_style(table_checkout, LV_TABLE_PART_BG,    &style_table);
  lv_obj_add_style(table_checkout, LV_TABLE_PART_CELL1, &style_cells);
  lv_obj_add_style(table_checkout, LV_TABLE_PART_CELL2, &style_cells);
  lv_obj_add_style(table_checkout, LV_TABLE_PART_CELL2, &(ui_controller->styles.font_16_normal));
  lv_obj_add_style(table_checkout, LV_TABLE_PART_CELL2, &style_cells_2);
  lv_table_set_col_cnt(table_checkout, 2);
  lv_table_set_row_cnt(table_checkout, 3);
  lv_table_set_col_width(table_checkout, 0, 120);
  lv_table_set_col_width(table_checkout, 1, 110);
  lv_obj_align(table_checkout, NULL, LV_ALIGN_CENTER, 0, -10);

  /* Cells */
  for (int i = 0; i < 3; i++) {
    lv_table_set_cell_align(table_checkout, i, 0, LV_LABEL_ALIGN_LEFT);
    lv_table_set_cell_align(table_checkout, i, 1, LV_LABEL_ALIGN_LEFT);
  }

  lv_table_set_cell_type(table_checkout, 0, 0, LV_TABLE_PART_CELL2);
  lv_table_set_cell_type(table_checkout, 0, 1, LV_TABLE_PART_CELL2);
  lv_table_set_cell_type(table_checkout, 1, 0, LV_TABLE_PART_CELL1);
  lv_table_set_cell_type(table_checkout, 1, 1, LV_TABLE_PART_CELL1);
  lv_table_set_cell_type(table_checkout, 2, 0, LV_TABLE_PART_CELL2);
  lv_table_set_cell_type(table_checkout, 2, 1, LV_TABLE_PART_CELL2);

  lv_table_set_cell_value(table_checkout, 0, 0, "Garrafões");
  lv_table_set_cell_value(table_checkout, 1, 0, "Copos");
  lv_table_set_cell_value(table_checkout, 2, 0, "Preço total");

  char aux_bottles[16];
  sprintf(aux_bottles, "%d un.", smart_ring_ui_get_controller()->order.bottles);
  char aux_cups[16];
  sprintf(aux_cups, "%dx100 un.", smart_ring_ui_get_controller()->order.cups);

  lv_table_set_cell_value(table_checkout, 0, 1, aux_bottles);
  lv_table_set_cell_value(table_checkout, 1, 1, aux_cups);
  lv_table_set_cell_value(table_checkout, 2, 1, "A processar");

  /* Checkout label */
  lv_obj_t *label_checkout = lv_label_create(parent, NULL);
  lv_label_set_text(label_checkout, "Checkout:");
  lv_obj_align(label_checkout, table_checkout, LV_ALIGN_OUT_TOP_MID, -4, -6);

  /********************
   *  VALIDATE BUTTON
   ********************/
  btn_check_order = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_check_order, smart_ring_ui_order_accept_order_handler);
  lv_obj_add_style(btn_check_order, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_check_order, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_check_order, table_checkout, LV_ALIGN_OUT_BOTTOM_MID, -60, 14);
  lv_obj_set_style_local_bg_color(btn_check_order,   LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xF7, 0xF7, 0xF7));
  lv_obj_set_style_local_text_color(btn_check_order, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_click(btn_check_order, false);

  label_check_order = lv_label_create(btn_check_order, NULL);
  lv_label_set_text(label_check_order, LV_SYMBOL_OK);
  lv_obj_set_hidden(label_check_order, true);

  // spinner for the check button
  spinner_check_order = lv_spinner_create(btn_check_order, NULL);
  lv_obj_set_size(spinner_check_order, 25, 25);
  lv_obj_align(spinner_check_order, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_local_line_width(spinner_check_order, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, 2);
  lv_obj_set_style_local_line_color(spinner_check_order, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_line_width(spinner_check_order, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 2);
  lv_obj_set_style_local_line_color(spinner_check_order, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_spinner_set_arc_length(spinner_check_order, 60);

  /******************
   *  CANCEL BUTTON
   ******************/
  static lv_style_t style_btn_red;
  lv_style_init(&style_btn_red);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);

  lv_obj_t *btn_cancel = lv_btn_create(parent, btn_check_order);
  lv_obj_set_event_cb(btn_cancel, smart_ring_ui_order_cancel_order_handler);
  lv_obj_add_style(btn_cancel, LV_BTN_PART_MAIN, &style_btn_red);
  lv_obj_align(btn_cancel, btn_check_order, LV_ALIGN_CENTER, 120, 0);
  lv_obj_set_style_local_bg_color(btn_cancel,   LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_obj_set_style_local_text_color(btn_cancel, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_click(btn_cancel, true);

  lv_obj_t *label_btn = lv_label_create(btn_cancel, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_CLOSE);
}

void order_create_processing_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  create_header(parent, "Nova encomenda", ui_controller);

  lv_obj_t *truck = lv_img_create(parent, NULL);

  lv_img_set_src(truck, &icon_truck);
  lv_img_set_zoom(truck, 328);

  lv_obj_align(truck, parent, LV_ALIGN_CENTER, 0, -40);

  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *label = lv_label_create(parent, NULL);

  lv_label_set_text(label, "A preparar a sua encomenda");
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_text);
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label, parent, LV_ALIGN_CENTER, 0, 30);

  label = lv_label_create(parent, NULL);
  lv_label_set_text(label, "Por favor aguarde...");
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_text);
  lv_obj_align(label, parent, LV_ALIGN_CENTER, 0, 60);
}

void order_create_successfull_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Nova encomenda", ui_controller);

  lv_obj_t *icon = lv_img_create(parent, NULL);
  lv_img_set_src(icon, &icon_delivery_ok);

  lv_img_set_zoom(icon, 300);

  lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, -50);

  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *label = lv_label_create(parent, NULL);

  lv_label_set_text(label, "Encomenda em processamento");
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &(ui_controller->styles.font_16_bold));
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &style_blue_text);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

  label = lv_label_create(parent, NULL);
  lv_label_set_text( label, "Consulte o menu \"Gerir Encomendas\" para saber o estado das mesmas");
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, 300);
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &style_blue_text);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 35);

  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, main_home_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_OK);
}

void order_create_canceled_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Nova encomenda", ui_controller);

  lv_obj_t *icon = lv_img_create(parent, NULL);
  lv_img_set_src(icon, &icon_delivery_error);

  lv_img_set_zoom(icon, 300);

  lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, -50);

  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *label = lv_label_create(parent, NULL);

  lv_label_set_text(label, "A encomenda foi cancelada");
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &(ui_controller->styles.font_16_bold));
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &style_blue_text);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

  label = lv_label_create(parent, NULL);
  lv_label_set_text(label, "Volte ao menu de encomendas");
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, 300);
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &style_blue_text);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 35);

  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, main_home_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_OK);
}

void order_create_error_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Nova encomenda", ui_controller);

  lv_obj_t *cont = lv_cont_create(parent, NULL);
  lv_obj_set_width(cont, 200);
  lv_obj_set_style_local_bg_opa(cont,     LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
  lv_obj_set_style_local_border_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
  lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, -40);

  lv_obj_t *icon = lv_img_create(cont, NULL);
  lv_img_set_src(icon, &icon_alert);
  lv_obj_align(icon, NULL, LV_ALIGN_CENTER, -40, 0);

  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *error_label = lv_label_create(cont, NULL);
  lv_label_set_text(error_label, "Error 001");
  lv_obj_add_style(error_label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_obj_add_style(error_label, LV_LABEL_PART_MAIN, &style_blue_text);

  lv_obj_align(error_label, icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

  lv_obj_t *label = lv_label_create(parent, NULL);

  lv_label_set_text(label, "Erro ao processar a encomenda");
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &(ui_controller->styles.font_16_bold));
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &style_blue_text);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 10);

  label = lv_label_create(parent, NULL);
  lv_label_set_text(label, "Volte ao menu de encomendas");
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, 300);
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &style_blue_text);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 35);

  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, main_home_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_OK);
}

void order_create_manage_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  lv_obj_t *page_scrollbar = lv_page_create(parent, NULL);
  lv_obj_set_size(page_scrollbar, lv_obj_get_width_fit(parent), lv_obj_get_height_fit(parent));
  lv_obj_align(page_scrollbar, NULL, LV_ALIGN_CENTER, 0, 0);

  create_header(parent, "Gerir encomendas", ui_controller);

  /**********
   *  TABLE
   **********/
  static lv_style_t style_cells;
  lv_style_init(&style_cells);
  lv_style_set_border_width(&style_cells, LV_STATE_DEFAULT, 1);
  lv_style_set_pad_top(&style_cells,      LV_STATE_DEFAULT, 6);
  lv_style_set_pad_bottom(&style_cells,   LV_STATE_DEFAULT, 6);
  lv_style_set_pad_right(&style_cells,    LV_STATE_DEFAULT, 2);
  lv_style_set_pad_left(&style_cells,     LV_STATE_DEFAULT, 2);
  lv_style_set_text_color(&style_cells,   LV_STATE_DEFAULT, LV_COLOR_MAKE(0x97, 0x97, 0x97));

  static lv_style_t style_cells_2;
  lv_style_init(&style_cells_2);
  lv_style_set_border_width(&style_cells_2, LV_STATE_DEFAULT, 0);

  static lv_style_t style_cells_header;
  lv_style_init(&style_cells_header);
  lv_style_set_bg_color(&style_cells_header,   LV_STATE_DEFAULT, LV_COLOR_MAKE(0xEB, 0xED, 0xF0));
  lv_style_set_bg_opa(&style_cells_header,     LV_STATE_DEFAULT, LV_OPA_100);
  lv_style_set_text_color(&style_cells_header, LV_STATE_DEFAULT, LV_COLOR_BLACK);

  static lv_style_t style_table;
  lv_style_init(&style_table);
  lv_style_set_border_width(&style_table, LV_STATE_DEFAULT, 1);
  lv_style_set_pad_inner(&style_table,    LV_STATE_DEFAULT, 0);
  lv_style_set_pad_right(&style_table,    LV_STATE_DEFAULT, 0);
  lv_style_set_pad_left(&style_table,     LV_STATE_DEFAULT, 0);
  lv_style_set_pad_top(&style_table,      LV_STATE_DEFAULT, 0);
  lv_style_set_pad_bottom(&style_table,   LV_STATE_DEFAULT, 0);

  /* Table */
  table_deliveries = lv_table_create(page_scrollbar, NULL);
  lv_obj_set_size(table_deliveries, lv_obj_get_width_fit(page_scrollbar), lv_obj_get_height_fit(page_scrollbar));

  lv_obj_add_style(table_deliveries, LV_TABLE_PART_BG,    &style_table);
  lv_obj_add_style(table_deliveries, LV_TABLE_PART_CELL1, &style_cells);
  lv_obj_add_style(table_deliveries, LV_TABLE_PART_CELL2, &style_cells);
  lv_obj_add_style(table_deliveries, LV_TABLE_PART_CELL2, &style_cells_2);
  lv_obj_add_style(table_deliveries, LV_TABLE_PART_CELL3, &style_cells);
  lv_obj_add_style(table_deliveries, LV_TABLE_PART_CELL3, &style_cells_2);
  lv_obj_add_style(table_deliveries, LV_TABLE_PART_CELL3, &style_cells_header);
  lv_obj_add_style(table_deliveries, LV_TABLE_PART_CELL3, &(ui_controller->styles.font_16_normal));

  lv_obj_align(table_deliveries, NULL, LV_ALIGN_IN_TOP_LEFT, -8,  20);

  lv_page_set_scrollbar_mode(page_scrollbar, LV_SCROLLBAR_MODE_ON | LV_SCROLLBAR_MODE_DRAG);
  lv_page_glue_obj(table_deliveries, true);

  lv_table_set_col_cnt(table_deliveries, 4);
  lv_table_set_row_cnt(table_deliveries, ui_controller->number_of_deliveries+1);
  lv_table_set_col_width(table_deliveries, 0, 100);
  lv_table_set_col_width(table_deliveries, 1, 64);
  lv_table_set_col_width(table_deliveries, 2, 60);
  lv_table_set_col_width(table_deliveries, 3, 75);

  /* Cells */
  for (int i = 0; i <= ui_controller->number_of_deliveries+1; i++) {
    lv_table_set_cell_align(table_deliveries, i, 0, LV_LABEL_ALIGN_CENTER);
    lv_table_set_cell_align(table_deliveries, i, 1, LV_LABEL_ALIGN_CENTER);
    lv_table_set_cell_align(table_deliveries, i, 2, LV_LABEL_ALIGN_CENTER);
    lv_table_set_cell_align(table_deliveries, i, 3, LV_LABEL_ALIGN_CENTER);
  }

  lv_table_set_cell_type(table_deliveries, 0, 0, LV_TABLE_PART_CELL3);
  lv_table_set_cell_type(table_deliveries, 0, 1, LV_TABLE_PART_CELL3);
  lv_table_set_cell_type(table_deliveries, 0, 2, LV_TABLE_PART_CELL3);
  lv_table_set_cell_type(table_deliveries, 0, 3, LV_TABLE_PART_CELL3);

  lv_table_set_cell_type(table_deliveries, 1, 0, LV_TABLE_PART_CELL1);
  lv_table_set_cell_type(table_deliveries, 1, 1, LV_TABLE_PART_CELL1);
  lv_table_set_cell_type(table_deliveries, 1, 2, LV_TABLE_PART_CELL1);
  lv_table_set_cell_type(table_deliveries, 1, 3, LV_TABLE_PART_CELL1);

  lv_table_set_cell_type(table_deliveries, 2, 0, LV_TABLE_PART_CELL1);
  lv_table_set_cell_type(table_deliveries, 2, 1, LV_TABLE_PART_CELL1);
  lv_table_set_cell_type(table_deliveries, 2, 2, LV_TABLE_PART_CELL1);
  lv_table_set_cell_type(table_deliveries, 2, 3, LV_TABLE_PART_CELL1);

  lv_table_set_cell_type(table_deliveries, 3, 0, LV_TABLE_PART_CELL2);
  lv_table_set_cell_type(table_deliveries, 3, 1, LV_TABLE_PART_CELL2);
  lv_table_set_cell_type(table_deliveries, 3, 2, LV_TABLE_PART_CELL2);
  lv_table_set_cell_type(table_deliveries, 3, 3, LV_TABLE_PART_CELL2);

  lv_table_set_cell_value(table_deliveries, 0, 0, "Data");
  lv_table_set_cell_value(table_deliveries, 0, 1, "Garraf.");
  lv_table_set_cell_value(table_deliveries, 0, 2, "Copos");
  lv_table_set_cell_value(table_deliveries, 0, 3, "Estado");

  // Add the deliveries content on the table
  if (ui_controller->deliveries != NULL) {
    page = 1;

    number_of_pages = (ui_controller->number_of_deliveries % 3 == 0 ? ui_controller->number_of_deliveries / 3
                    : (ui_controller->number_of_deliveries + (3 - (ui_controller->number_of_deliveries % 3))) / 3);

#ifndef NDEBUG
    ESP_LOGI(TAG, "Number of deliveries : %d\nNumber of pages : %d",
             ui_controller->number_of_deliveries, number_of_pages);
#endif

    for (int i = 0; i < ui_controller->number_of_deliveries; i++) {
      lv_table_set_cell_value_fmt(table_deliveries, i + 1, 0, "%s",     ui_controller->deliveries[i].date);
      lv_table_set_cell_value_fmt(table_deliveries, i + 1, 1, "    %d", ui_controller->deliveries[i].bottles);
      lv_table_set_cell_value_fmt(table_deliveries, i + 1, 2, "%dx100", ui_controller->deliveries[i].cups);
      lv_table_set_cell_value_fmt(table_deliveries, i + 1, 3, "%s",     ui_controller->deliveries[i].status);
    }

    lv_table_set_cell_value_fmt(table_deliveries, ui_controller->number_of_deliveries+1, 0, " ");
    lv_table_set_cell_value_fmt(table_deliveries, ui_controller->number_of_deliveries+1, 1, " ");
    lv_table_set_cell_value_fmt(table_deliveries, ui_controller->number_of_deliveries+1, 2, " ");
    lv_table_set_cell_value_fmt(table_deliveries, ui_controller->number_of_deliveries+1, 3, " ");

  }

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
  lv_obj_set_style_local_outline_opa(img_btn_home, LV_BTN_PART_MAIN, LV_STATE_FOCUSED,      LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_home,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_home, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -7);

  lv_obj_t *img_btn_home_img = lv_img_create(img_btn_home, NULL);
  lv_img_set_src(img_btn_home_img, &icon_home);

  // free(ui_controller->deliveries);
}

void order_create_error_manage_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Gerir encomendas", ui_controller);

  lv_obj_t *cont = lv_cont_create(parent, NULL);
  lv_obj_set_width(cont, 200);
  lv_obj_set_style_local_bg_opa(cont,     LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
  lv_obj_set_style_local_border_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
  lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, -60);

  lv_obj_t *icon = lv_img_create(cont, NULL);
  lv_img_set_src(icon, &icon_alert);
  lv_obj_align(icon, NULL, LV_ALIGN_CENTER, -40, 0);

  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *error_label = lv_label_create(cont, NULL);
  lv_label_set_text(error_label, "Erro 002");
  lv_obj_add_style(error_label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_obj_add_style(error_label, LV_LABEL_PART_MAIN, &style_blue_text);

  lv_obj_align(error_label, icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

  lv_obj_t *label = lv_label_create(parent, NULL);

  lv_label_set_text(label, "Erro ao receber lista de encomendas");
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_label_set_align(label,     LV_LABEL_ALIGN_CENTER);
  lv_obj_set_width(label, 300);
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &(ui_controller->styles.font_16_bold));
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &style_blue_text);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -5);

  label = lv_label_create(parent, NULL);
  lv_label_set_text(label, "Por favor tente outra vez");
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, 300);
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &(ui_controller->styles.font_14_normal));
  lv_obj_add_style(label,   LV_IMG_PART_MAIN, &style_blue_text);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 30);

  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, smart_ring_ui_check_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_OK);
}

void order_create_change_order_mode_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  create_header(parent, "Modo de encomenda", ui_controller);

  // static lv_style_t style_delivery_mode_btns;
  // lv_style_init(&style_delivery_mode_btns);
  // lv_style_set_bg_color(&style_delivery_mode_btns, LV_STATE_CHECKED,
  //                       LV_COLOR_WHITE);
  // lv_style_set_bg_color(&style_delivery_mode_btns, LV_STATE_FOCUSED,
  //                       LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  // lv_style_set_text_color(&style_delivery_mode_btns, LV_STATE_FOCUSED,
  //                         LV_COLOR_WHITE);
  // lv_style_set_bg_color(&style_delivery_mode_btns, LV_STATE_PRESSED,
  //                       LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  // lv_style_set_text_color(&style_delivery_mode_btns, LV_STATE_PRESSED,
  //                         LV_COLOR_WHITE);

  // lv_obj_t *btn_auto = lv_btn_create(parent, NULL);
  // // lv_obj_set_event_cb(btn_auto, btn_auto_mode_event_handler);
  // lv_obj_add_style(btn_auto, LV_OBJ_PART_MAIN,
  //                  &(ui_controller->styles.white_button));
  // lv_obj_add_style(btn_auto, LV_OBJ_PART_MAIN, &style_delivery_mode_btns);
  // lv_obj_add_style(btn_auto, LV_OBJ_PART_MAIN,
  //                  &(ui_controller->styles.font_16_normal));
  // lv_obj_set_size(btn_auto, SR_DEFAULT_BTN_WIDTH * 3 / 2,
  //                 SR_DEFAULT_BTN_HEIGHT * 3 / 2);
  // lv_obj_align(btn_auto, NULL, LV_ALIGN_CENTER, 0, -40);

  // lv_obj_t *label = lv_label_create(btn_auto, NULL);
  // lv_label_set_text(label, "Automático");

  // lv_obj_t *btn_manual = lv_btn_create(parent, btn_auto);
  // // lv_obj_set_event_cb(btn_manual, btn_manual_mode_event_handler);
  // lv_obj_align(btn_manual, NULL, LV_ALIGN_CENTER, 0, 30);

  // label = lv_label_create(btn_manual, NULL);
  // lv_label_set_text(label, "Manual");

  // if (smart_ring_ui_get_order_mode() == 'a') {
  //     lv_obj_set_style_local_bg_color(btn_auto, LV_BTN_PART_MAIN,
  //                                     LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00,
  //                                     0xA9, 0xE0));
  //     lv_obj_set_style_local_text_color(btn_auto, LV_BTN_PART_MAIN,
  //                                       LV_STATE_DEFAULT, LV_COLOR_WHITE);
  // } else {
  //     lv_obj_set_style_local_bg_color(btn_manual, LV_BTN_PART_MAIN,
  //                                     LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00,
  //                                     0xA9, 0xE0));
  //     lv_obj_set_style_local_text_color(btn_manual, LV_BTN_PART_MAIN,
  //                                       LV_STATE_DEFAULT, LV_COLOR_WHITE);
  // }

  // /*****************
  //  *  Back button  *
  //  *****************/
  // lv_obj_t *img_btn_back = lv_btn_create(parent, NULL);
  // // lv_obj_set_event_cb(img_btn_back, sr_ui_btn_logout_event_handler);
  // lv_btn_set_fit(img_btn_back, LV_FIT_TIGHT);
  // lv_obj_set_style_local_pad_all(img_btn_back, LV_BTN_PART_MAIN,
  //                                LV_BTN_STATE_RELEASED, 10);
  // lv_obj_set_style_local_radius(img_btn_back, LV_BTN_PART_MAIN,
  //                               LV_BTN_STATE_RELEASED, 0);
  // lv_obj_set_style_local_border_opa(img_btn_back, LV_BTN_PART_MAIN,
  //                                   LV_BTN_STATE_RELEASED, LV_OPA_0);
  // lv_obj_set_style_local_bg_opa(img_btn_back, LV_BTN_PART_MAIN,
  //                               LV_BTN_STATE_RELEASED, LV_OPA_100);
  // lv_obj_set_style_local_bg_color(img_btn_back, LV_BTN_PART_MAIN,
  //                                 LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  // lv_obj_set_style_local_outline_opa(img_btn_back, LV_BTN_PART_MAIN,
  //                                    LV_STATE_FOCUSED, LV_OPA_0);
  // lv_obj_set_style_local_bg_color(img_btn_back, LV_BTN_PART_MAIN,
  //                                 LV_STATE_PRESSED | LV_STATE_FOCUSED,
  //                                 LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  // lv_obj_align(img_btn_back, NULL, LV_ALIGN_IN_BOTTOM_MID, 100, -5);

  // lv_obj_t *img_btn_back_img = lv_img_create(img_btn_back, NULL);
  // lv_img_set_src(img_btn_back_img, &icon_logout);

  // /****************
  //  *  Home button
  //  ****************/
  // lv_obj_t *img_btn_home = lv_btn_create(parent, NULL);
  // lv_obj_set_event_cb(img_btn_home, main_home_handler);
  // lv_btn_set_fit(img_btn_home, LV_FIT_TIGHT);
  // lv_obj_set_style_local_pad_all(img_btn_home, LV_BTN_PART_MAIN,
  //                                LV_BTN_STATE_RELEASED, 10);
  // lv_obj_set_style_local_radius(img_btn_home, LV_BTN_PART_MAIN,
  //                               LV_BTN_STATE_RELEASED, 0);
  // lv_obj_set_style_local_border_opa(img_btn_home, LV_BTN_PART_MAIN,
  //                                   LV_BTN_STATE_RELEASED, LV_OPA_0);
  // lv_obj_set_style_local_bg_opa(img_btn_home, LV_BTN_PART_MAIN,
  //                               LV_BTN_STATE_RELEASED, LV_OPA_100);
  // lv_obj_set_style_local_bg_color(img_btn_home, LV_BTN_PART_MAIN,
  //                                 LV_BTN_STATE_RELEASED, LV_COLOR_WHITE);
  // lv_obj_set_style_local_outline_opa(img_btn_home, LV_BTN_PART_MAIN,
  //                                    LV_STATE_FOCUSED, LV_OPA_0);
  // lv_obj_set_style_local_bg_color(img_btn_home, LV_BTN_PART_MAIN,
  //                                 LV_STATE_PRESSED | LV_STATE_FOCUSED,
  //                                 LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  // lv_obj_align(img_btn_home, NULL, LV_ALIGN_IN_BOTTOM_MID, -100, -5);

  // lv_obj_t *img_btn_home_img = lv_img_create(img_btn_home, NULL);
  // lv_img_set_src(img_btn_home_img, &icon_home);

  lv_obj_t *toggle = lv_switch_create(parent, NULL);

  if (smart_ring_ui_get_order_mode() == 'a') {
    lv_switch_on(toggle, LV_ANIM_OFF);
  }

  lv_obj_align(toggle, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_local_bg_color(toggle, LV_SWITCH_PART_INDIC,
                                  LV_STATE_DEFAULT,
                                  LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
  lv_obj_set_size(toggle, 55, 25);

  lv_obj_set_event_cb(toggle, order_change_mode_handler);

  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT,
                          LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *label = lv_label_create(parent, NULL);

  lv_label_set_text(label, "Manual");
  lv_obj_add_style(label,     LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_obj_add_style(label,     LV_LABEL_PART_MAIN, &style_blue_text);
  lv_label_set_align(label,   LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label, toggle, LV_ALIGN_OUT_LEFT_MID, -15, 0);

  label = lv_label_create(parent, NULL);

  lv_label_set_text(label, "Automático");
  lv_obj_add_style(label,     LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_bold));
  lv_obj_add_style(label,     LV_LABEL_PART_MAIN, &style_blue_text);
  lv_label_set_align(label,   LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label, toggle, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

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
  lv_obj_set_style_local_outline_opa(img_btn_home, LV_BTN_PART_MAIN, LV_STATE_FOCUSED,      LV_OPA_0);
  lv_obj_set_style_local_bg_color(img_btn_home,    LV_BTN_PART_MAIN, LV_STATE_PRESSED | LV_STATE_FOCUSED, LV_COLOR_MAKE(0xF0, 0xF0, 0xF0));
  lv_obj_align(img_btn_home, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -15);

  lv_obj_t *img_btn_home_img = lv_img_create(img_btn_home, NULL);
  lv_img_set_src(img_btn_home_img, &icon_home);
}

void order_create_successfull_change_mode_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  create_header(parent, "Modo de encomenda", ui_controller);

  static lv_style_t style_blue_text;
  lv_style_init(&style_blue_text);
  lv_style_set_text_color(&style_blue_text, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *label = lv_label_create(parent, NULL);
  if (ui_controller->order_mode == 'a') {
    lv_label_set_text(label, "Modo de encomenda alterado para automático");
  } else {
    lv_label_set_text(label, "Modo de encomenda alterado para manual");
  }
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, 300);
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -20);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_blue_text);
  lv_obj_add_style(label, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));

  lv_obj_t *btn_validate = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_validate, smart_ring_ui_check_handler);
  lv_obj_add_style(btn_validate, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_validate, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_validate, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

  lv_obj_t *label_btn = lv_label_create(btn_validate, NULL);
  lv_label_set_text(label_btn, LV_SYMBOL_OK);
}

static void smart_ring_ui_new_order_popup_handler(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {

#ifndef NDEBUG
    ESP_LOGI(TAG, "Button: %s\n", lv_msgbox_get_active_btn_text(obj));
#endif

    smart_ring_ui_get_controller()->standby_controller.has_callback = true; 

    if (strcmp("Continuar", lv_msgbox_get_active_btn_text(obj)) == 0) 
        smart_ring_ui_update_state(STATE_22);
    
    else 
        smart_ring_ui_update_state(STATE_11);
  }
}

void order_popup_pending_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
  order_create_new_order_screen(parent, ui_controller);

  if (ui_controller->number_of_deliveries > 0) {
    char *order_popup_text = calloc(120, sizeof(char));
    lv_obj_t *new_order_messagebox = lv_msgbox_create(parent, NULL);

  
    if(ui_controller->number_of_deliveries >= 5) {
      sprintf(order_popup_text, "Ainda tem %d pedidos pendentes. Por favor, ligue para o atendimento ao cliente.", ui_controller->number_of_deliveries);
      smart_ring_ui_get_controller()->standby_controller.has_callback = true;
      smart_ring_ui_update_state(STATE_22);
    }

    else {
       sprintf(order_popup_text, "Ainda tem %d pedidos pendentes.", ui_controller->number_of_deliveries);

       static lv_style_t style_msgbox_buttons;
       lv_style_init(&style_msgbox_buttons);
       lv_style_set_border_color(&style_msgbox_buttons, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
    
      

       static const char *order_popup_buttons[] = {"Continuar", "Cancelar", ""};
       lv_msgbox_add_btns(new_order_messagebox, order_popup_buttons);
       lv_obj_add_style(new_order_messagebox, LV_MSGBOX_PART_BTN, &style_msgbox_buttons);
       lv_obj_set_event_cb(new_order_messagebox, smart_ring_ui_new_order_popup_handler);

#ifndef NDEBUG
       ESP_LOGI(TAG, "%s", order_popup_text);
#endif
    }
    lv_obj_set_width(new_order_messagebox, 300);
    lv_obj_align(new_order_messagebox, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_msgbox_set_text(new_order_messagebox, order_popup_text);
    free(order_popup_text);
  }
  else {
#ifndef NDEBUG
    ESP_LOGI(TAG, "No pending deliveries");
#endif
    smart_ring_ui_get_controller()->standby_controller.has_callback = true;
    smart_ring_ui_update_state(STATE_22);
  }

}