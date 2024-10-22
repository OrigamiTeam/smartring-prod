/**
 * @file libs.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Main library files, to group all dependecies together
 * @version 2.0
 * @date 2022-06-22
 *
 * @copyright Copyright (c) 2023
 *
 
 ___        __                 __         
/\_ \    __/\ \               /\ \        
\//\ \  /\_\ \ \____    ____  \ \ \___    
  \ \ \ \/\ \ \ '__`\  /',__\  \ \  _ `\  
   \_\ \_\ \ \ \ \L\ \/\__, `\__\ \ \ \ \ 
   /\____\\ \_\ \_,__/\/\____/\_\\ \_\ \_\
   \/____/ \/_/\/___/  \/___/\/_/ \/_/\/_/                                         
                                          
 
 */

#ifndef __LIBS_H_
#define __LIBS_H_

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "esp_wifi.h"
#include "esp_sntp.h"
#include "sys/param.h"

#include "lvgl_esp32_drivers/lvgl_touch/touch_driver.h"

#include "mqtt_client.h"

#include "esp_ota_ops.h"
#include "esp_tls.h"

#include "esp_crt_bundle.h"
#include "esp_http_client.h"
// test end

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_spiffs.h"

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_version.h"

//#include "tcpip_adapter.h"

#include "driver/gpio.h"

#include "cJSON.h"

#include "HX711_ADC.h"
#include "hx711.h"

/*** UI ***/
#include "lvgl.h"
#include "lvgl_helpers.h"
#include "smart_ring_ui.h"

#include "http_client.h"
#include "http_server.h"
#include "mqtt.h"
#include "nvs.h"
#include "spiffs.h"
#include "sensors.h"
#include "sleep.h"
#include "vars.h"
#include "wifi.h"
#include "uiflag_app.h"
#include "sntp.h"
#include "sntpprotocol.h"

#endif /* __LIBS_H_ */