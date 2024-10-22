/**
 * @file http_client.h
 * @authors  Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief HTTP client header
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
  

 __      __    __                            ___                       __        __         
/\ \    /\ \__/\ \__                        /\_ \    __               /\ \__    /\ \        
\ \ \___\ \ ,_\ \ ,_\  _____             ___\//\ \  /\_\     __    ___\ \ ,_\   \ \ \___    
 \ \  _ `\ \ \/\ \ \/ /\ '__`\          /'___\\ \ \ \/\ \  /'__`\/' _ `\ \ \/    \ \  _ `\  
  \ \ \ \ \ \ \_\ \ \_\ \ \L\ \        /\ \__/ \_\ \_\ \ \/\  __//\ \/\ \ \ \_  __\ \ \ \ \ 
   \ \_\ \_\ \__\\ \__\\ \ ,__/        \ \____\/\____\\ \_\ \____\ \_\ \_\ \__\/\_\\ \_\ \_\
    \/_/\/_/\/__/ \/__/ \ \ \/   _______\/____/\/____/ \/_/\/____/\/_/\/_/\/__/\/_/ \/_/\/_/
                         \ \_\  /\______\                                                   
                          \/_/  \/______/                                                   

 */

#ifndef __HTTP_CLIENT_H_
#define __HTTP_CLIENT_H_

#include "libs.h"

/// Check firmware request type
#define HTTP_CLIENT_GET_CHECK_FIRMWARE 0

/// Get firmware request type
#define HTTP_CLIENT_GET_UPDATE_FIRMWARE 1

/// API Endpoint the device will connect to
#define API_ENDPOINT      "https://s23tn26sz7.execute-api.eu-central-1.amazonaws.com/stage"

/// API KEY the devices will use
#define API_KEY           "dGt7ldTurh6hu5ZiuXjBm9udAsOBHlLA54JpEPJX"

#define MQTT_API_ENDPOINT "https://k8jge8j6yj.execute-api.eu-central-1.amazonaws.com/prod/device-claim-cert-api"
#define MQTT_API_KEY        "iVH3vQc2GUaYPQ7loplFr6lf9rtGpvNO3C56dIzR"

#define MAX_HTTP_RECV_BUFFER   512
#define MAX_HTTP_OUTPUT_BUFFER 4096


/// Certificate start for the OTA update
extern const uint8_t ota_root_pem_start[] asm("_binary_ota_root_pem_start");

/// Certificate end for the OTA update
extern const uint8_t ota_root_pem_end[] asm("_binary_ota_root_pem_end");

/**
 * @brief
 * Check if there is a new firmware version available in the server
 *
 */
void smart_ring_http_client_get_check_firmware();

/**
 * @brief
 * Update the device firmware to the latest on the server
 *
 */
void smart_ring_http_client_get_update_firmware();

void smart_ring_http_client_get_certificate(void);

#endif