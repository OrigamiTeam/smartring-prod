/**
 * @file stock.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Responsible for the stock related ui screens and functions
 * @version 2.0
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2023
 *
 * 

███████╗████████╗ ██████╗  ██████╗██╗  ██╗    ██████╗
██╔════╝╚══██╔══╝██╔═══██╗██╔════╝██║ ██╔╝   ██╔════╝
███████╗   ██║   ██║   ██║██║     █████╔╝    ██║     
╚════██║   ██║   ██║   ██║██║     ██╔═██╗    ██║     
███████║   ██║   ╚██████╔╝╚██████╗██║  ██╗██╗╚██████╗
╚══════╝   ╚═╝    ╚═════╝  ╚═════╝╚═╝  ╚═╝╚═╝ ╚═════╝
                                                                                                      

 */

#include "stock.h"

void stock_update_success_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
     create_header(parent, "Correção de stock", ui_controller);

     static lv_style_t style_blue_font;
     lv_style_init(&style_blue_font);
     lv_style_set_text_color(&style_blue_font, LV_STATE_DEFAULT,LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

     lv_obj_t *title = lv_label_create(parent, NULL);
     lv_obj_add_style(title,  LV_LABEL_PART_MAIN, &style_blue_font);
     lv_obj_add_style(title,  LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
     lv_label_set_text(title, "Número de Garrafões atualizado com sucesso!");
     lv_label_set_long_mode(title, LV_LABEL_LONG_BREAK);
     lv_label_set_align(title,     LV_LABEL_ALIGN_CENTER);
     lv_obj_set_width(title, 250);
     lv_obj_align(title, NULL, LV_ALIGN_CENTER, 0, -20);

     lv_obj_t *button_continue = lv_btn_create(parent, NULL);
     lv_obj_set_event_cb(button_continue, smart_ring_ui_check_handler);
     lv_obj_add_style(button_continue, LV_BTN_PART_MAIN, &(ui_controller->styles.blue_button));
     lv_obj_set_size(button_continue, 140, 40);
     lv_obj_align(button_continue, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -15);

     lv_obj_t *label = lv_label_create(button_continue, NULL);
     lv_label_set_text(label, "Continuar");
     lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}