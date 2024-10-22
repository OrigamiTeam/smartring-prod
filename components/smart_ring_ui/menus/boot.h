/**
 * @file boot.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Boot menu header
 * @version 2.1.1
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __BOOT_H_
#define __BOOT_H_

#include "lvgl.h"
#include "smart_ring_ui.h"

/**
 * @brief
 * Update the warning label text on the boot screen
 *
 * @param text - Message to be written on screen
 */
void boot_update_warning_label(char *text);

/**
 * @brief
 * Update the boot screen, hide the buttons and update the label
 *
 */
void boot_update_connection_screen();

/**
 * @brief
 * Create booting up screen with logo and information
 *
 */
void create_booting_up_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief
 * Create the boot menu where the communication is selected
 *
 */
void create_selected_communication(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief
 * Create the screen for the user to choose the communication type
 * pretended
 *
 */
void create_communication_selection(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

#endif // __BOOT_H