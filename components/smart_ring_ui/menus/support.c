/**
 * @file support.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Responsible for the support request screens
 * @version 2.0
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2023
 *
 * 
 

                                                             $$\                  
                                                             $$ |                 
 $$$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\   $$$$$$\   $$$$$$\ $$$$$$\       $$$$$$$\ 
$$  _____|$$ |  $$ |$$  __$$\ $$  __$$\ $$  __$$\ $$  __$$\\_$$  _|     $$  _____|
\$$$$$$\  $$ |  $$ |$$ /  $$ |$$ /  $$ |$$ /  $$ |$$ |  \__| $$ |       $$ /      
 \____$$\ $$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |       $$ |$$\    $$ |      
$$$$$$$  |\$$$$$$  |$$$$$$$  |$$$$$$$  |\$$$$$$  |$$ |       \$$$$  |$$\\$$$$$$$\ 
\_______/  \______/ $$  ____/ $$  ____/  \______/ \__|        \____/ \__|\_______|
                    $$ |      $$ |                                                
                    $$ |      $$ |                                                
                    \__|      \__|                                                
                                                                                  
                                                                                  
 */

#include "support.h"

LV_IMG_DECLARE(icon_home)

void support_create_review(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  create_header(parent, "Pedido de suporte", ui_controller);

  lv_obj_t *label = lv_label_create(parent, NULL);
  lv_obj_add_style(label, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_text(label, "Motivo a reportar:");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -68);

  lv_obj_t *cont_ticket = lv_cont_create(parent, NULL);
  lv_obj_add_style(cont_ticket, LV_CONT_PART_MAIN, &(ui_controller->styles.white_button));
  lv_obj_set_width(cont_ticket, 130);

  lv_obj_t *label_report = lv_label_create(cont_ticket, NULL);
  lv_obj_add_style(label_report, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_long_mode(label_report, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label_report, 130);

  switch (ui_controller->support_type) {
  case BAD_TASTE_WATER: lv_label_set_text(label_report, "Água com mau sabor");          break;
  case BOTH_TAPS_COLD:  lv_label_set_text(label_report, "Faz frio nas duas torneiras");  break;
  case NO_COLD:         lv_label_set_text(label_report, "Não faz frio");              break;
  case LEAKING_WATER:   lv_label_set_text(label_report, "Verte água");              break;
  case OTHER:           lv_label_set_text(label_report, "Outro");                      break;
  default:              lv_label_set_text(label_report, "Erro no pedido");                break;
  }

  lv_label_set_align(label_report, LV_LABEL_ALIGN_CENTER);
  lv_cont_set_fit(cont_ticket, LV_FIT_TIGHT);
  lv_obj_align(cont_ticket, NULL, LV_ALIGN_CENTER, 0, 0);

  /**************
   *  OK Button
   **************/
  lv_obj_t *btn_ok = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_ok, smart_ring_ui_check_handler);
  lv_obj_add_style(btn_ok, LV_BTN_PART_MAIN,  &(ui_controller->styles.blue_button));
  lv_obj_set_size(btn_ok, SR_DEFAULT_BTN_WIDTH, SR_DEFAULT_BTN_HEIGHT);
  lv_obj_align(btn_ok, parent, LV_ALIGN_IN_BOTTOM_MID, 0, -18);

  label = lv_label_create(btn_ok, NULL);
  lv_label_set_text(label, "Reportar");

  /****************
   *  Home Button
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
  lv_obj_align(img_btn_home, NULL, LV_ALIGN_IN_BOTTOM_MID, -100, -5);

  lv_obj_t *img_btn_home_img = lv_img_create(img_btn_home, NULL);
  lv_img_set_src(img_btn_home_img, &icon_home);
}

void support_create_submited(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {

  create_header(parent, "Pedido de suporte", ui_controller);

  static lv_style_t style_blue_font;
  lv_style_init(&style_blue_font);
  lv_style_set_text_color(&style_blue_font, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

  lv_obj_t *title = lv_label_create(parent, NULL);
  lv_obj_add_style(title, LV_LABEL_PART_MAIN, &style_blue_font);
  lv_obj_add_style(title, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
  lv_label_set_text(title, "Pedido registado com sucesso");
  lv_label_set_long_mode(title, LV_LABEL_LONG_BREAK);
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_width(title, 200);
  lv_obj_align(title, NULL, LV_ALIGN_CENTER, 0, -40);

  lv_obj_t *main_text = lv_label_create(parent, NULL);

  lv_obj_add_style(main_text, LV_LABEL_PART_MAIN, &style_blue_font);
  lv_obj_add_style(main_text, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
  lv_label_set_text(main_text, "Aguarde contacto por parte da Fonte Viva.");
  lv_label_set_long_mode(main_text, LV_LABEL_LONG_BREAK);
  lv_label_set_align(main_text, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_width(main_text, 200);
  lv_obj_align(main_text, NULL, LV_ALIGN_CENTER, 0, 15);

  lv_obj_t *button_continue = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(button_continue, smart_ring_ui_check_handler);
  lv_obj_add_style(button_continue, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
  lv_obj_set_size(button_continue, 140, 40);
  lv_obj_align(button_continue, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -15);

  lv_obj_t *label = lv_label_create(button_continue, NULL);
  lv_label_set_text(label, "Continuar");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}