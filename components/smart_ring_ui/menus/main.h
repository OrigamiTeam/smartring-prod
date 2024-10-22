/**
 * @file main.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Main screen header
 * @version 2.0
 * @date 2022-06-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __MAIN_H_
#define __MAIN_H_

#include "lvgl.h"
#include "smart_ring_ui.h"


typedef enum smart_ring_support_type_t {
    BAD_TASTE_WATER,
    BOTH_TAPS_COLD,
    NO_COLD,
    LEAKING_WATER,
    OTHER
};

typedef struct smart_ring_support_option_t {
    enum smart_ring_support_type_t type;
    char                           text[32];
};

void smart_ring_ui_check_handler(lv_obj_t *button, lv_event_t event);
void main_logout_handler(lv_obj_t *button, lv_event_t event);

void smart_ring_ui_main_update_stock_value(int stock);

/**
 * @brief  Create the main screen
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void main_create_screen(lv_obj_t  *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief  Create the dashboard screen
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void main_create_dashboard_menu(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

void smart_ring_main_debug_update_sensor_values(int old_value, int new_value, bool stable);

/**
 * @brief  Create the debug screen
 * @note   It may change during the course of the firmware
 * integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void main_create_debug_menu(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief  Create the reset device or wifi menu
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void main_create_reset_menu(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief Create the device information screen
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void main_create_info_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);
void main_home_handler(lv_obj_t *button, lv_event_t event);
void main_update_rssi_signal(uint8_t rssi_level);
void main_update_device_name(char *device_name);
void main_update_eta_information(char *information);
void main_update_bottle_icon(struct smart_ring_ui_controller_t *ui_controller);
void main_update_stock(int stock);
void main_update_order_mode(char order_mode);
void smart_ring_ui_create_sleep_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);
void smart_ring_ui_main_update_next_eta();
void smart_ring_ui_main_clear_next_eta();

#endif /* __MAIN_H_ */