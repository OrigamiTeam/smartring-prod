/**
 * @file calibration.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * @version 2.0
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2023
 *
 * 
 * 
            _ _ _               _   _               _     
   ___ __ _| (_) |__  _ __ __ _| |_(_) ___  _ __   | |__  
  / __/ _` | | | '_ \| '__/ _` | __| |/ _ \| '_ \  | '_ \ 
 | (_| (_| | | | |_) | | | (_| | |_| | (_) | | | |_| | | |
  \___\__,_|_|_|_.__/|_|  \__,_|\__|_|\___/|_| |_(_)_| |_|
                                                          

 */

#ifndef __CALIBRATION_H_
#define __CALIBRATION_H_

#include "lvgl.h"
#include "smart_ring_ui.h"

/**
 * @brief  Create the calibration screen to ask the user to confirm that there
 * is no bottle inserted
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void calibration_create_no_bottle_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief  Create the calibration screen to ask the user to confirm that there
 * is a full bottle inserted
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void calibration_create_full_bottle_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

void smart_ring_ui_calibration_update_progress_screen(bool finished);

/**
 * @brief  Create in progress screen to show during the
 * calibration step
 * @note It may change during the course of the firmware
 * integration
 * @param *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void calibration_create_progress_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief  Create the finished calibration screen to inform the user if the
 * calibration was successful
 * @note It may change during the course of the firmware integration
 * @param *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void calibration_create_finished_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);
#endif