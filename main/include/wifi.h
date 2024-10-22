/**
 * @file wifi.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Wifi header
 * @version 2.1.2.
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
                  ___          __         
            __  /'___\ __     /\ \        
 __  __  __/\_\/\ \__//\_\    \ \ \___    
/\ \/\ \/\ \/\ \ \ ,__\/\ \    \ \  _ `\  
\ \ \_/ \_/ \ \ \ \ \_/\ \ \  __\ \ \ \ \ 
 \ \___x___/'\ \_\ \_\  \ \_\/\_\\ \_\ \_\
  \/__//__/   \/_/\/_/   \/_/\/_/ \/_/\/_/
                                                                                   
 */

#ifndef __WIFI_H_
#define __WIFI_h_

/**
 * @brief
 * Initialize the wifi interface and thread for the wireless
 * communication
 *
 * @param  *connection_controller: Connection object
 */
void wifi_interface_start(struct smart_ring_connection_controller_t *connection_controller);

/**
 * @brief
 * Configure the access point to provision the device
 *
 * @param  *connection_controller:  connection object
 */
static void
wifi_ap_init(struct smart_ring_connection_controller_t *connection_controller);

/**
 * @brief
 * Get the wifi rssi level
 *
 * @retval rssi RSSI level of the device
 */
int8_t wifi_get_rssi();

/**
 * @brief
 * Delete the wifi credentials from the NVS storage
 *
 */
void wifi_reset_device();
#endif