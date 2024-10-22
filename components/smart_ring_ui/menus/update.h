/**
 * @file update.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @authors
 * @brief Firmware update screens header
 * @version 2.0.1
 * @date 2022-09-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __UPDATE_H_
#define __UPDATE_H_

#include "smart_ring_ui.h"


void smart_ring_ui_update_download_started(void);
void smart_ring_ui_update_change_percentage(int percentage);
void update_create_firmware_progress_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);
void update_create_firmware_finished_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller, bool success);
void update_create_option_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);
#endif