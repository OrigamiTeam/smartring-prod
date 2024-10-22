/**
 * @file pin.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Pin header
 * @version 2.0
 * @date 2022-06-23
 *
 * @copyright Copyright (c) 2023
 *
 * 
 
 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄        ▄     ▄         ▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░▌      ▐░▌   ▐░▌       ▐░▌
▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ ▐░▌░▌     ▐░▌   ▐░▌       ▐░▌
▐░▌       ▐░▌     ▐░▌     ▐░▌▐░▌    ▐░▌   ▐░▌       ▐░▌
▐░█▄▄▄▄▄▄▄█░▌     ▐░▌     ▐░▌ ▐░▌   ▐░▌   ▐░█▄▄▄▄▄▄▄█░▌
▐░░░░░░░░░░░▌     ▐░▌     ▐░▌  ▐░▌  ▐░▌   ▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀      ▐░▌     ▐░▌   ▐░▌ ▐░▌   ▐░█▀▀▀▀▀▀▀█░▌
▐░▌               ▐░▌     ▐░▌    ▐░▌▐░▌   ▐░▌       ▐░▌
▐░▌           ▄▄▄▄█░█▄▄▄▄ ▐░▌     ▐░▐░▌ ▄ ▐░▌       ▐░▌
▐░▌          ▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░▌▐░▌       ▐░▌
 ▀            ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀  ▀  ▀         ▀ 
                                                       

 */

/**
 * @brief  Pin screen header
 * @note
 */
#ifndef __PIN_H_
#define __PIN_H_

#include "lvgl.h"
#include "smart_ring_ui.h"

char *pin_get_inserted_pin();

/**
 * @brief  Create the pin screen with the keyboard
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void create_insert_pin_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

#endif /* __PIN_H_ */