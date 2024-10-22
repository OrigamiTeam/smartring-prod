/**
 * @file modal.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Modal screens header
 * @version 2.0
 * @date 2022-06-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __MODAL_H_
#define __MODAL_H_

#include "smart_ring_ui.h"

/**
 @brief Create the provisioning modal with the network name
 * @note It may change during the course of the firmware integration
 * @param *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @retval None
 */
void modal_create_provision(lv_obj_t                          *parent,
                            struct smart_ring_ui_controller_t *ui_controller);

/**
 * @brief Create a modal using the parameters defined
 * @note   It may change during the course of the firmware integration
 * @param  *parent: Screen object
 * @param  *ui_controller: UI controller object
 * @param  *title: Title for the modal
 * @param  *subheader: Text for the modal
 * @param  spinner: If the modal uses a spinner
 * @retval None
 */
void modal_create_template(lv_obj_t                          *parent,
                           struct smart_ring_ui_controller_t *ui_controller,
                           char *title, char *subheader, bool spinner);

/**
 * @brief  Change the modal information as the operation was completed
 * successfully
 * @note   It may change during the course of the firmware integration
 * @param  *ui_controller: UI controller object
 * @param  successfull: If the operation was successfull
 * @retval None
 */
void modal_complete_progress(struct smart_ring_ui_controller_t *ui_controller,
                             bool successfull, char *error_message);
#endif /* __MODAL_H_ */