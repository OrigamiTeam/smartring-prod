/**
 * @file reset.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Reset screen functions header
 * @version 2.0.1
 * @date 2022-09-07
 *
 * @copyright Copyright (c) 2023
 *
 * 
 
 .S_sSSs      sSSs    sSSs    sSSs  sdSS_SSSSSSbs         .S    S.   
.SS~YS%%b    d%%SP   d%%SP   d%%SP  YSSS~S%SSSSSP        .SS    SS.  
S%S   `S%b  d%S'    d%S'    d%S'         S%S             S%S    S%S  
S%S    S%S  S%S     S%|     S%S          S%S             S%S    S%S  
S%S    d*S  S&S     S&S     S&S          S&S             S%S SSSS%S  
S&S   .S*S  S&S_Ss  Y&Ss    S&S_Ss       S&S             S&S  SSS&S  
S&S_sdSSS   S&S~SP  `S&&S   S&S~SP       S&S             S&S    S&S  
S&S~YSY%b   S&S       `S*S  S&S          S&S             S&S    S&S  
S*S   `S%b  S*b        l*S  S*b          S*S             S*S    S*S  
S*S    S%S  S*S.      .S*P  S*S.         S*S             S*S    S*S  
S*S    S&S   SSSbs  sSS*S    SSSbs       S*S             S*S    S*S  
S*S    SSS    YSSP  YSS'      YSSP       S*S        SS   SSS    S*S  
SP                                       SP        S%%S         SP   
Y                                        Y          SS          Y    
                                                                     

 */

#ifndef __RESET_H_
#define __RESET_H_

#include "smart_ring_ui.h"

void reset_create_device_screen(lv_obj_t   *parent, struct smart_ring_ui_controller_t *ui_controller);

void reset_create_wifi_screen(lv_obj_t     *parent, struct smart_ring_ui_controller_t *ui_controller);

void reset_create_progress_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller);

void reset_create_finished_screen(lv_obj_t *parent, struct smart_ring_ui_controller_t *ui_controller, bool success);

#endif