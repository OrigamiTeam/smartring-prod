/**
 * @file support.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * @version 2.0
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2023
 *
 * 

                                                             $$\         $$\       
                                                             $$ |        $$ |      
 $$$$$$$\ $$\   $$\  $$$$$$\   $$$$$$\   $$$$$$\   $$$$$$\ $$$$$$\       $$$$$$$\  
$$  _____|$$ |  $$ |$$  __$$\ $$  __$$\ $$  __$$\ $$  __$$\\_$$  _|      $$  __$$\ 
\$$$$$$\  $$ |  $$ |$$ /  $$ |$$ /  $$ |$$ /  $$ |$$ |  \__| $$ |        $$ |  $$ |
 \____$$\ $$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |       $$ |$$\     $$ |  $$ |
$$$$$$$  |\$$$$$$  |$$$$$$$  |$$$$$$$  |\$$$$$$  |$$ |       \$$$$  |$$\ $$ |  $$ |
\_______/  \______/ $$  ____/ $$  ____/  \______/ \__|        \____/ \__|\__|  \__|
                    $$ |      $$ |                                                 
                    $$ |      $$ |                                                 
                    \__|      \__|                                                 
                                                                                   
                                                                                                             
 */

#ifndef __SUPPORT_H_
#define __SUPPORT_H_

#include "lvgl.h"
#include "smart_ring_ui.h"

/**
 * @brief  Create the support review screen so the user can confirm the type of
 * help he's requesting
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void support_create_review(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief  Create the screen to inform the user the report was submitted
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void support_create_submited(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);
#endif