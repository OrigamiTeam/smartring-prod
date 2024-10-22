/**
 * @file modal.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Responsible for the creation of the different modals in the interface
 * @version 2.0
 * @date 2022-06-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "modal.h"

static lv_obj_t *spinner;
static lv_obj_t *spinner_icon;
static lv_obj_t *modal_title;
static lv_obj_t *modal_subheader;

void modal_create_provision(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller) {
    static lv_style_t style_backdrop;
    lv_style_init(&style_backdrop);
    lv_style_set_bg_color(&style_backdrop, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x80, 0x80));
    lv_style_set_border_width(&style_backdrop, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_opa(&style_backdrop,       LV_STATE_DEFAULT, LV_OPA_70);
    lv_style_set_radius(&style_backdrop,       LV_STATE_DEFAULT, 0);

    lv_obj_t *cont_ap_modal_backdrop = lv_cont_create(parent, NULL);
    lv_obj_add_style(cont_ap_modal_backdrop,   LV_OBJ_PART_MAIN, &style_backdrop);
    lv_cont_set_layout(cont_ap_modal_backdrop, LV_LAYOUT_OFF);
    lv_obj_set_size(cont_ap_modal_backdrop, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_align(cont_ap_modal_backdrop, NULL, LV_ALIGN_CENTER, 0, 0);

    static lv_style_t style_modal;
    lv_style_init(&style_modal);
    lv_style_set_text_color(&style_modal,   LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
    lv_style_set_border_color(&style_modal, LV_STATE_DEFAULT, LV_COLOR_RED);

    lv_obj_t *cont_modal = lv_cont_create(cont_ap_modal_backdrop, NULL);
    lv_obj_add_style(cont_modal, LV_OBJ_PART_MAIN, &style_modal);
    lv_obj_set_size(cont_modal, 272, 204);
    lv_obj_align(cont_modal, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_move_foreground(cont_modal);

    lv_obj_t *label_title = lv_label_create(cont_modal, NULL);
    lv_obj_add_style(label_title, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_22_normal));
    char *mac_address = smart_ring_ui_get_mac_address();
    lv_label_set_text_fmt(label_title, "SR_%c%c%c%c", mac_address[12], mac_address[13], mac_address[15], mac_address[16]);
    lv_label_set_align(label_title, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label_title, NULL, LV_ALIGN_CENTER, 0, -10);

    lv_obj_t *label_aux_boot = lv_label_create(cont_modal, NULL);
    lv_obj_add_style(label_aux_boot, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_18_normal));
    lv_label_set_align(label_aux_boot, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(label_aux_boot, "Ligue-se à rede");
    lv_obj_align(label_aux_boot, NULL, LV_ALIGN_CENTER, 0, -50);

    label_aux_boot = lv_label_create(cont_modal, NULL);
    lv_obj_add_style(label_aux_boot, LV_OBJ_PART_MAIN, &(ui_controller->styles.font_18_normal));
    lv_label_set_align(label_aux_boot, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(label_aux_boot, "e siga os passos\nna aplicação");
    lv_obj_align(label_aux_boot, NULL, LV_ALIGN_CENTER, 0, 40);
}


void modal_create_template(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller,
                                             char *title, char *subheader, bool has_spinner) {
    lv_obj_t *background = lv_obj_create(parent, NULL);
    lv_obj_set_size(background, 320, 240);
    lv_obj_align(background, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_style_local_bg_color(background, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_opa(background,   LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_30);
    lv_obj_set_style_local_radius(background,   LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    static lv_style_t style_modal;
    lv_style_init(&style_modal);
    lv_style_set_text_color(&style_modal, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));

    lv_obj_t *modal = lv_obj_create(background, NULL);
    lv_obj_add_style(modal, LV_OBJ_PART_MAIN, &style_modal);
    lv_obj_set_size(modal, 280, 200);
    lv_obj_align(modal, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_color(modal, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    if (has_spinner) {
        spinner = lv_spinner_create(modal, NULL);
        lv_obj_set_size(spinner, 65, 65);
        lv_obj_align(spinner, NULL, LV_ALIGN_CENTER, 0, -40);
        lv_obj_set_style_local_line_width(spinner, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, 4);
        lv_obj_set_style_local_line_color(spinner, LV_SPINNER_PART_BG,    LV_STATE_DEFAULT, LV_COLOR_MAKE(0xE0, 0xE0, 0xE0));
        lv_obj_set_style_local_line_width(spinner, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 4);
        lv_obj_set_style_local_line_color(spinner, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0xA9, 0xE0));
        lv_spinner_set_arc_length(spinner, 60);

        spinner_icon = lv_label_create(spinner, NULL);
        lv_label_set_text(spinner_icon, LV_SYMBOL_OK);
        lv_obj_align(spinner_icon, spinner, LV_ALIGN_CENTER, -3, -3);
        lv_obj_set_hidden(spinner_icon, false);
        lv_obj_set_style_local_text_font(spinner_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_22);
        lv_obj_set_hidden(spinner_icon, true);
    }

    static lv_style_t style_header;
    lv_style_init(&style_header);
    lv_style_set_text_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_GRAY);

    modal_title = lv_label_create(modal, NULL);
    lv_obj_add_style(modal_title, LV_LABEL_PART_MAIN, &style_header);
    lv_obj_add_style(modal_title, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_18_bold));
    lv_label_set_long_mode(modal_title, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(modal_title, 280);
    lv_label_set_align(modal_title, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(modal_title, title);
    if (has_spinner) {
        lv_obj_align(modal_title, NULL, LV_ALIGN_CENTER, 0, 30);
    } else {
        lv_obj_align(modal_title, NULL, LV_ALIGN_CENTER, 0, -10);
    }

    printf("CREATING SUBTITLE\n");
    static lv_style_t style_sub_label;
    lv_style_init(&style_sub_label);
    lv_style_set_text_color(&style_sub_label, LV_STATE_DEFAULT, LV_COLOR_GRAY);

    modal_subheader = lv_label_create(modal, NULL);
    lv_obj_add_style(modal_subheader, LV_LABEL_PART_MAIN, &style_sub_label);
    lv_obj_add_style(modal_subheader, LV_LABEL_PART_MAIN, &(ui_controller->styles.font_16_normal));
    lv_label_set_long_mode(modal_subheader, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(modal_subheader, 280);
    lv_label_set_align(modal_subheader, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(modal_subheader, subheader);
    if (has_spinner) {
        lv_obj_align(modal_subheader, NULL, LV_ALIGN_CENTER, 0, 55);
    } else {
        lv_obj_align(modal_subheader, NULL, LV_ALIGN_CENTER, 0, 15);
    }
}


void modal_complete_progress(struct smart_ring_ui_controller_t *ui_controller, bool successfull, 
                                                                         char *error_message) {

    // Complete the spinner
    lv_spinner_set_arc_length(spinner, 359);

    // Show the icon and change the text
    if (successfull) {
        lv_label_set_text(spinner_icon, LV_SYMBOL_OK);
        lv_obj_set_hidden(spinner_icon, false);

        lv_label_set_text(modal_title, ui_controller->flags.flag.update_pin ? "Atualizado" : "Concluído");
        lv_label_set_text(modal_subheader, "Aguarde 3 segundos");
        ui_controller->flags.flag.update_pin  = false;
    } else {
        lv_label_set_text(spinner_icon, LV_SYMBOL_CLOSE);
        lv_obj_set_style_local_text_color(spinner_icon, LV_LABEL_PART_MAIN,   LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_obj_set_style_local_line_color(spinner,      LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_obj_align(spinner_icon, spinner, LV_ALIGN_CENTER, 0, 0);

        lv_obj_set_hidden(spinner_icon, false);
        lv_label_set_text(modal_title, "Erro");
        lv_label_set_text(modal_subheader, error_message);
    }
}