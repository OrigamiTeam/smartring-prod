/**
 * @file sntp.h
 * @author Mateus Araujo @ Quantum Leap (mateus@quleap.com)
 * @brief
 * This file contains the functions responsible for the Simple Network Time Protocol (SNTP)
 *
 * @version 2.1.2
 * @date 2024-04-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#define TIME_PERC_INTERVAL  2000 // Time interval to check SNTP Time 

/**
 * @brief
 * Function responsible to initialize SNTP Protocol
 */
void initialize_sntp();

/**
 * @brief
 * Function to wait for SNTP Sync
 */
void wait_for_sntp_sync();

/**
 * @brief
 * Function responsible to set the timezone
 */
void set_timezone();

/**
 * @brief
 * Alert function to confirm SNTP Sync
 *
 * @param tv
 */
void time_sync_notification_cb(struct timeval *tv);

/**
 * @brief
 * Function to verify scheduled time to send Percentage Consumption
 *
 * @param controller
 * @return true OR false
 */
bool isTimePerc(struct smart_ring_controller_t *controller);

/**
 * @brief
 * Function to verify scheduled time to send Version Check
 *
 * @param controller
 * @return true OR false
 */
bool versionCheck(struct smart_ring_controller_t *controller);