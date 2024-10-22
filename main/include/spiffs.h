/**
 * @file spiffs.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the initialization and
 * update of the SPIFFs storage portion of the device
 *
 *
 * @version -
 * @date 2023-05-03
 *
 * @copyright Copyright (c) 2023
                     ___    ___          __         
               __  /'___\ /'___\        /\ \        
  ____  _____ /\_\/\ \__//\ \__/  ____  \ \ \___    
 /',__\/\ '__`\/\ \ \ ,__\ \ ,__\/',__\  \ \  _ `\  
/\__, `\ \ \L\ \ \ \ \ \_/\ \ \_/\__, `\__\ \ \ \ \ 
\/\____/\ \ ,__/\ \_\ \_\  \ \_\\/\____/\_\\ \_\ \_\
 \/___/  \ \ \/  \/_/\/_/   \/_/ \/___/\/_/ \/_/\/_/
          \ \_\                                     
           \/_/                                     

 */



#ifndef __SPIFFS_H
#define __SPIFFS_H


#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"


typedef struct spiffs_t {
       void (*saveMQTTCertificatePrivateKey) (char *key);
       void (*loadMQTTCertificatePrivateKey)(char*);
}spiffs_t;

extern spiffs_t spiffs;
#endif