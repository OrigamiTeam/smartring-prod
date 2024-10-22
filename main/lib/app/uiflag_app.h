/**
 * @file uiFlag_app.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the UIflag readability
 *
 *
 * @version -
 * @date 2023-05-03
 *
 * @copyright Copyright (c) 2023
 *
 */




#ifndef __UI_FLAG_H
#define __UI_FLAG_H

#include "lvgl.h"
#include "lvgl_helpers.h"
#include "smart_ring_ui.h"

#include "http_client.h"
#include "http_server.h"
#include "mqtt.h"
#include "nvs.h"
#include "sensors.h"
#include "sleep.h"
#include "vars.h"
#include "wifi.h"

#define UI_FLAG_CODE_NOT_USED  1

typedef struct ui_flag_t {
        void (*startLogin)(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
        void (*selectCommunication)(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
        void (*startCommunication)(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
        void (*signIn)(struct smart_ring_controller_t*);
        void (*sendTicket)(struct smart_ring_controller_t*);
        void (*changeStock)(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
        void (*popupPendingOrders)(struct smart_ring_controller_t*);
        void (*changeOrderMode)(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
        void (*manageOrders)(struct smart_ring_controller_t*);
        void (*resetDevice)(struct smart_ring_controller_t*);
        void (*resetWifi)(struct smart_ring_controller_t*);
        void (*changePin)(struct smart_ring_controller_t*);
        void (*updateFirmware)(struct smart_ring_controller_t*);
        void (*updateFirmwareFailed)(struct smart_ring_controller_t *);
        void (*updateFirmwareComplete)(struct smart_ring_controller_t*);
        void (*startOrder)(struct smart_ring_controller_t*);
        void (*startCalibration)(struct smart_ring_controller_t*); 
        void (*emptyCalibration)(struct smart_ring_controller_t*);
        void (*cancelCalibration)(struct smart_ring_controller_t*);
        void (*confirmOrder)(struct smart_ring_controller_t*); 
}ui_flag_t;


extern ui_flag_t uiflag;


#endif