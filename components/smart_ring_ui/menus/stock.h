/**
 * @file stock.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Stock header
 * @version 2.0
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2023
 *
 * 

███████╗████████╗ ██████╗  ██████╗██╗  ██╗   ██╗  ██╗
██╔════╝╚══██╔══╝██╔═══██╗██╔════╝██║ ██╔╝   ██║  ██║
███████╗   ██║   ██║   ██║██║     █████╔╝    ███████║
╚════██║   ██║   ██║   ██║██║     ██╔═██╗    ██╔══██║
███████║   ██║   ╚██████╔╝╚██████╗██║  ██╗██╗██║  ██║
╚══════╝   ╚═╝    ╚═════╝  ╚═════╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝
                                                                                                       

 */

#ifndef __STOCK_H_
#define __STOCK_H_

#include "lvgl.h"
#include "smart_ring_ui.h"

/**
 * @brief  Screen to inform the user if the stock was successfully or not
 * updated
 * @note It may change during the course of the fimrware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void stock_update_success_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

#endif