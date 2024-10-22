/**
 * @file sleep.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the standby mode of the
 * project
 *
 *
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "libs.h"

static void sleep_timer_callback(void *arg) {

  // Enter light sleep
}

void setup_sleep_mode() {

  struct smart_ring_controller_t *controller = smart_ring_get_controller();
  // Create the timer instance to be used
  const esp_timer_create_args_t sleep_timer_args = {
      .callback = &sleep_timer_callback,
      /* argument specified here will be passed to timer callback function
       */
      .name = "ui_controller_timer",
  };

  esp_timer_create(&sleep_timer_args, &controller->sleep_timer);

  // Start the timer for x seconds
  esp_timer_start_once(controller->sleep_timer,
                       controller->sleep_timeout * 1000000);
}