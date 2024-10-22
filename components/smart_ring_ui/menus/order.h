/**
 * @file order.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Order screens header
 * @version 2.0
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2023
 *
 * 

_____________________________________/\\\______________________________________/\\\_________        
 ____________________________________\/\\\_____________________________________\/\\\_________       
  ____________________________________\/\\\_____________________________________\/\\\_________      
   _____/\\\\\_____/\\/\\\\\\\_________\/\\\______/\\\\\\\\___/\\/\\\\\\\________\/\\\_________     
    ___/\\\///\\\__\/\\\/////\\\___/\\\\\\\\\____/\\\/////\\\_\/\\\/////\\\_______\/\\\\\\\\\\__    
     __/\\\__\//\\\_\/\\\___\///___/\\\////\\\___/\\\\\\\\\\\__\/\\\___\///________\/\\\/////\\\_   
      _\//\\\__/\\\__\/\\\_________\/\\\__\/\\\__\//\\///////___\/\\\_______________\/\\\___\/\\\_  
       __\///\\\\\/___\/\\\_________\//\\\\\\\/\\__\//\\\\\\\\\\_\/\\\__________/\\\_\/\\\___\/\\\_ 
        ____\/////_____\///___________\///////\//____\//////////__\///__________\///__\///____\///__

 */

#ifndef __ORDER_H_
#define __ORDER_H_

#include "lvgl.h"
#include "smart_ring_ui.h"

/**
 * @brief  Create the new order screen
 * @note   It may change during the course of the fimrware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void order_create_new_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief  Create the review order screen
 * @note   It may change during the course of the fimrware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void order_create_review_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief  Create the processing order screen
 * @note   It may change during the course of the fimrware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void order_create_processing_order_screen(lv_obj_t *parent,   struct smart_ring_ui_controller_t *ui_controller);

void order_create_successfull_order_screen(lv_obj_t *parent,  struct smart_ring_ui_controller_t *ui_controller);

void order_create_canceled_order_screen(lv_obj_t *parent,     struct smart_ring_ui_controller_t *ui_controller);

void order_create_error_order_screen(lv_obj_t *parent,        struct smart_ring_ui_controller_t *ui_controller);

void order_create_manage_order_screen(lv_obj_t *parent,       struct smart_ring_ui_controller_t *ui_controller);

void order_create_error_manage_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

void order_create_change_order_mode_screen(lv_obj_t *parent,  struct smart_ring_ui_controller_t *ui_controller);

void order_create_successfull_change_mode_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

void smart_ring_ui_order_update_review_data(float price);
void order_popup_pending_order_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

#endif