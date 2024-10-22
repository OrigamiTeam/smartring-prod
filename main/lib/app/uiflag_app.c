/**
 * @file   uiflag_app.c
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


#include "uiflag_app.h"
#include "sntp.h"
#include "libs.h"

void uiflag_startLogin(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
void uiflag_selectCommunication(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
void uiflag_startCommunication(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
void uiflag_signIn(struct smart_ring_controller_t*);
void uiflag_changePin(struct smart_ring_controller_t *);
void uiflag_sendTicket(struct smart_ring_controller_t*);
void uiflag_changeStock(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
void uiflag_popupPendingOrders (struct smart_ring_controller_t*);
void uiflag_changeOrderMode(struct smart_ring_controller_t*, struct smart_ring_ui_controller_t*);
void uiflag_manageOrders(struct smart_ring_controller_t*);
void uiflag_resetDevice(struct smart_ring_controller_t*);
void uiflag_resetWifi(struct smart_ring_controller_t*);
void uiflag_updateFirmware(struct smart_ring_controller_t *);
void uiflag_updateFirmwareFailed(struct smart_ring_controller_t*);
void uiflag_updateFirmwareComplete(struct smart_ring_controller_t*);
void uiflag_startOrder(struct smart_ring_controller_t*);
void uiflag_startCalibration(struct smart_ring_controller_t*);
void uiflag_emptyCalibration(struct smart_ring_controller_t*);
void uiflag_cancelCalibration(struct smart_ring_controller_t*);
void uiflag_confirmOrder(struct smart_ring_controller_t*); 


static const char *TAG = "MAIN";

 ui_flag_t uiflag = {
           .startLogin             =  uiflag_startLogin,
           .selectCommunication    =  uiflag_selectCommunication,
           .startCommunication     =  uiflag_startCommunication,
           .signIn                 =  uiflag_signIn,
           .sendTicket             =  uiflag_sendTicket,
           .changeStock            =  uiflag_changeStock,
           .popupPendingOrders     =  uiflag_popupPendingOrders,
           .changeOrderMode        =  uiflag_changeOrderMode,
           .manageOrders           =  uiflag_manageOrders,
           .resetDevice            =  uiflag_resetDevice,
           .resetWifi              =  uiflag_resetWifi,
           .changePin              =  uiflag_changePin,
           .updateFirmware         =  uiflag_updateFirmware,
           .updateFirmwareFailed   =  uiflag_updateFirmwareFailed,
           .updateFirmwareComplete =  uiflag_updateFirmwareComplete,
           .startOrder             =  uiflag_startOrder,
           .startCalibration       =  uiflag_startCalibration,
           .emptyCalibration       =  uiflag_emptyCalibration,
           .cancelCalibration      =  uiflag_cancelCalibration,
           .confirmOrder           =  uiflag_confirmOrder,

 };

 
void uiflag_startLogin(struct smart_ring_controller_t *controller, struct smart_ring_ui_controller_t *uiController) {
     if (uiController->flags.flag.start_login) {
         if (controller->connection.is_connected)  {
             smart_ring_ui_get_controller()->flags.flag.update_pin  = false;
             smart_ring_ui_update_state(STATE_7);
         }
         else  
             smart_ring_ui_update_state(STATE_6);
         uiController->flags.flag.start_login = false;
     }
}


void uiflag_selectCommunication(struct smart_ring_controller_t *controller, struct smart_ring_ui_controller_t *uiController) {
     if (uiController->flags.flag.select_communication) {
         controller->connection.type = uiController->connection_type;
         nvs_save_connection_type(controller->connection.type);
         smart_ring_ui_update_state(STATE_2);
         uiController->flags.flag.select_communication = false;
     }
}


void uiflag_startCommunication(struct smart_ring_controller_t *controller, struct smart_ring_ui_controller_t *uiController) {
     if (uiController->flags.flag.start_communication) {

         // Initiate the Communication interface
         switch (controller->connection.type) {
              case 'w':
                        if (!controller->connection.is_provisioned) smart_ring_ui_update_state(STATE_3);
                        else smart_ring_ui_update_state(STATE_4);
                        wifi_interface_start(&(controller->connection));
                        break;
              case 'g':
                        break;
              case 'l':
                        break;
              default:
                        ESP_LOGE(TAG, "No communication type selected");
                        break;
         }
         uiController->flags.flag.start_communication = false;
      }
}


void uiflag_signIn(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.sign_in) {
         mqtt_send_message(SEND_LOGIN);
         controller->ui_controller->flags.flag.sign_in = false;
     }
}


void uiflag_changePin(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.pin_changed) {
         mqtt_send_message(SEND_CHANGE_PIN);
         controller->ui_controller->flags.flag.pin_changed = false;
     }
}


void uiflag_sendTicket(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.send_ticket) {
         int err = mqtt_send_message(SEND_TICKET);

        if (SUCCESS != err && MQTT_REQUEST_TIMEOUT_ERROR != err) {
            ESP_LOGE(TAG, "Error sending support message");
            modal_complete_progress(controller->ui_controller, false, "Error sending request");

            controller->ui_controller->timer_type = CLOSE_MODAL;
            esp_timer_start_once(controller->ui_controller->timer, 1000000 * 3);
        } 
        else
            smart_ring_ui_update_state(STATE_17);

        controller->ui_controller->flags.flag.send_ticket = false;
      }
}


void uiflag_changeStock(struct smart_ring_controller_t *controller, struct smart_ring_ui_controller_t *uiController) {
     if (controller->ui_controller->flags.flag.change_stock) {
         int err = mqtt_send_message(SEND_STOCK);

         if (SUCCESS != err && MQTT_REQUEST_TIMEOUT_ERROR != err) {
             ESP_LOGE(TAG, "Error sending stock message");
             modal_complete_progress(controller->ui_controller, false, "Error sending request");

             controller->ui_controller->timer_type = CLOSE_MODAL;
             esp_timer_start_once(controller->ui_controller->timer, 1000000 * 3);
         } 
         else {
             smart_ring_set_stock(uiController->updated_stock);
             smart_ring_ui_update_state(STATE_14);
         }
         controller->ui_controller->flags.flag.change_stock = false;
     }
}


void uiflag_popupPendingOrders (struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.order_popup_message) {
#ifndef NDEBUG
         ESP_LOGI(TAG, "Sending request to get pending order(s)");
#endif
         int err = mqtt_send_message(SEND_GET_ORDERS);

         if (SUCCESS != err && MQTT_REQUEST_TIMEOUT_ERROR != err) {
#ifndef NDEBUG
             ESP_LOGE(TAG, "Error Sending request to get pending order(s)");
#endif
             smart_ring_ui_update_state(STATE_46);
         }

         controller->ui_controller->flags.flag.order_popup_message = false;
     }
}


void uiflag_changeOrderMode(struct smart_ring_controller_t *controller, struct smart_ring_ui_controller_t *uiController) {
     if (controller->ui_controller->flags.flag.change_order_mode) {
         int err = mqtt_send_message(SEND_ORDER_MODE);

         if (SUCCESS != err && MQTT_REQUEST_TIMEOUT_ERROR != err) {
             ESP_LOGE(TAG, "Error sending stock message");
             modal_complete_progress(controller->ui_controller, false, "Error communicating");

             controller->ui_controller->timer_type = CLOSE_MODAL;
             esp_timer_start_once(controller->ui_controller->timer, 1000000 * 3);
         } 
         else {
             smart_ring_set_order_mode(uiController->updated_order_mode);
             smart_ring_ui_update_state(STATE_21);
         }
         controller->ui_controller->flags.flag.change_order_mode = false;
     }
}


void uiflag_manageOrders(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.manage_orders) {
         esp_timer_stop(controller->ui_controller->standby_timer);

         int err = mqtt_send_message(SEND_GET_ORDERS);

         if (SUCCESS != err && MQTT_REQUEST_TIMEOUT_ERROR != err) {
             ESP_LOGE(TAG, "Error sending request orders message");
             smart_ring_ui_update_state(STATE_19);
             esp_timer_stop(controller->ui_controller->timer);
         }
         controller->ui_controller->flags.flag.manage_orders = false;
     }
}


void uiflag_resetDevice(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.reset_device) {
         esp_restart();
         controller->ui_controller->flags.flag.reset_device = false;
     }
}


void uiflag_resetWifi(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.reset_wifi) {
         esp_err_t err = nvs_clear_wifi_credentials();
         if (ESP_OK == err) smart_ring_ui_update_state(STATE_40);
         else smart_ring_ui_update_state(STATE_41);
         controller->ui_controller->flags.flag.reset_wifi = false;
     }
}


void uiflag_updateFirmware(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.update_firmware) {
         controller->connection.mqtt_controller.auto_reconnect = false;
         aws_iot_mqtt_autoreconnect_set_status(&controller->connection.mqtt_controller.client, false);
         aws_iot_mqtt_disconnect(&controller->connection.mqtt_controller.client);
         vTaskDelay(500 / portTICK_PERIOD_MS);
         smart_ring_http_client_get_update_firmware();
        
         controller->ui_controller->flags.flag.update_firmware = false;
     }
}


void uiflag_updateFirmwareFailed(struct smart_ring_controller_t *controller) {
    if (controller->flags.flag.update_failed) {
        controller->connection.mqtt_controller.auto_reconnect = true;
        aws_iot_mqtt_autoreconnect_set_status(&controller->connection.mqtt_controller.client, true);
        controller->ui_controller->flags.flag.cancel_update = true;
        controller->flags.flag.update_failed                = false;
    }
}


void uiflag_updateFirmwareComplete(struct smart_ring_controller_t *controller) {
     if (controller->flags.flag.update_complete) {
          controller->connection.mqtt_controller.auto_reconnect = true;
          aws_iot_mqtt_autoreconnect_set_status(&controller->connection.mqtt_controller.client, true);
          controller->ui_controller->flags.flag.success = true;
          controller->flags.flag.update_complete        = false;
     }
}


void uiflag_startOrder(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.start_order) {
         smart_ring_ui_update_state(STATE_23);
         mqtt_send_message(SEND_NEW_DELIVERY);
         controller->ui_controller->flags.flag.start_order = false;
     }
}


void uiflag_startCalibration(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.start_calibration) {
         controller->sensor.calibration.timestamp                = esp_timer_get_time();
         controller->ui_controller->flags.flag.start_calibration = false;
         controller->sensor.calibration.calibrating              = true;
         controller->sensor.calibration.step                     = 1;
         controller->ui_controller->flags.flag.start_calibration = false;
     }
}


void uiflag_emptyCalibration(struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.empty_calibration) {
         controller->sensor.calibration.step        = 2;
         controller->sensor.calibration.calibrating = true;
         controller->sensor.calibration.timestamp   = esp_timer_get_time();
         controller->ui_controller->flags.flag.empty_calibration = false;
     }
}


void uiflag_cancelCalibration (struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.cancel_calibration) {
         controller->sensor.calibration.calibrating               = false;
         controller->sensor.calibration.number_of_readings        = 0;
         controller->sensor.calibration.step                      = 0;
         controller->sensor.calibration.sum_of_readings           = 0;
         controller->sensor.calibration.timestamp                 = -1;
         controller->ui_controller->flags.flag.cancel_calibration = false;
     }
}


void uiflag_confirmOrder (struct smart_ring_controller_t *controller) {
     if (controller->ui_controller->flags.flag.confirm_order) {
          mqtt_send_message(SEND_CONFIRM_DELIVERY); 
          int __timer = 0;

          while(!controller->ui_controller->flags.flag.order_confirmed_response) {
             vTaskDelay(100 / portTICK_PERIOD_MS);
             if(__timer++ > 5) break;
          }
          mqtt_send_message(SEND_GET_ORDERS);
          controller->ui_controller->flags.flag.order_confirmed_response = false;
          controller->ui_controller->flags.flag.confirm_order            = false;
    }
}



