/**
 * @file sensors.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the configuration and
 * reading of the loadcells
 *
 *
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
                                                     __         
                                                    /\ \        
  ____     __    ___     ____    ___   _ __   ____  \ \ \___    
 /',__\  /'__`\/' _ `\  /',__\  / __`\/\`'__\/',__\  \ \  _ `\  
/\__, `\/\  __//\ \/\ \/\__, `\/\ \L\ \ \ \//\__, `\__\ \ \ \ \ 
\/\____/\ \____\ \_\ \_\/\____/\ \____/\ \_\\/\____/\_\\ \_\ \_\
 \/___/  \/____/\/_/\/_/\/___/  \/___/  \/_/ \/___/\/_/ \/_/\/_/
                                                                                                                           
 */

#ifndef __SENSORS_H_
#define __SENSORS_H_

#include "libs.h"


/**
 * @brief
 * need to differ from the last_sensor_sent by ACCEPTED_INTERVAL
 * The value **240** is considered as about 1 Liter as requested by Marco Dias.
 *
 * ***
 *
 * Considering:
 *
 * ```
 *  Full tank (Raw value) : 89711
 *  Empty tank (Raw value) : 85179
 *
 *  Difference = 89711 - 85179 = 4532
 *
 *  1 Bottle = 18.9 Liters = 4532
 *
 *  1 Liter = 4532 / 18.9 = 239.78 = 240 (Raw)
 * ```
 */

#define ACCEPTED_INTERVAL         240
#define FULL_LITRES               18.9   
#define DIVISION_LITRES           10

/// The device needs to stay the same STATIC_MAX_COUNTER times in a row to be
/// re-sent
#define STATIC_MAX_COUNTER 15

/// The value is considered stable if the variation is below the
/// STABILIZED_INTERVAL
#define STABILIZED_INTERVAL 15

/**
 * @brief
 * Thread resposible for controlling the reading of the sensors
 *
 * @param pvParameter
 */
void smart_ring_sensors_task(void *pvParameter);

#endif