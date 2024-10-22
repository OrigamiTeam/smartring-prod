/**
 * @file http_server.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief HTTP Server header
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 

 __      __    __                                                                    __         
/\ \    /\ \__/\ \__                                                                /\ \        
\ \ \___\ \ ,_\ \ ,_\  _____             ____     __   _ __   __  __     __   _ __  \ \ \___    
 \ \  _ `\ \ \/\ \ \/ /\ '__`\          /',__\  /'__`\/\`'__\/\ \/\ \  /'__`\/\`'__\ \ \  _ `\  
  \ \ \ \ \ \ \_\ \ \_\ \ \L\ \        /\__, `\/\  __/\ \ \/ \ \ \_/ |/\  __/\ \ \/ __\ \ \ \ \ 
   \ \_\ \_\ \__\\ \__\\ \ ,__/        \/\____/\ \____\\ \_\  \ \___/ \ \____\\ \_\/\_\\ \_\ \_\
    \/_/\/_/\/__/ \/__/ \ \ \/   _______\/___/  \/____/ \/_/   \/__/   \/____/ \/_/\/_/ \/_/\/_/
                         \ \_\  /\______\                                                       
                          \/_/  \/______/                                                       

 */

#ifndef __HTTP_SERVER_H
#define __HTTP_SERVER_H

// HTTP Server task
/// Stack size for server task
#define HTTP_SERVER_TASK_STACK_SIZE 8192
/// Priority of the server task
#define HTTP_SERVER_TASK_PRIORITY 4
/// The task runs on the Core 0 (APP core)
#define HTTP_SERVER_TASK_CORE_ID 0

/**
 * @brief
 * Status of the provision operation, to send to the user web browser.
 *
 */
typedef enum smart_ring_provision_status_t {
  /**
   * @brief
   * The device is trying to connecto to network with the given credentials
   *
   */
  PROVISION_CONNECTING,

  /**
   * @brief
   * The device connected to the network
   *
   */
  PROVISION_SUCCESSFUL,

  /**
   * @brief
   * The device couldn't connect to the network
   *
   */
  PROVISION_FAILED
};

/**
 * @brief
 * Configure the HTTP server, start the handle and register all the files URI's.
 *
 * @return esp_err_t
 * @retval ESP_OK Configured successfully
 */
static esp_err_t http_server_configure(void);

/**
 * @brief
 * Start the HTTP server (only if there is no no server running) and return
 * the result of the operation
 *
 * @return esp_err_t - Result of the creation of the HTTP server
 */
esp_err_t http_server_start();

/**
 * @brief
 * Stop the HTTP server
 *
 */
void http_server_stop(void);

/**
 * @brief
 * Set the current provision state to the given one.
 * Used by the wifi controller to send to client browser after provision.
 *
 * @param state
 */
void provision_set_status(int state);

/**
 * @brief
 * Get the current provision status
 *
 * @return int
 */
int provision_get_status(void);

#endif