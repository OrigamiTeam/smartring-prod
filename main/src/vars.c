/**
 * @file vars.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for maintaining the system
 * variables
 *
 *
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "libs.h"

/* Main controller */
static struct smart_ring_controller_t controller = {
    .first_boot = false,
    .connection =
        {
            .type = 'w',
            .is_connected = false,
            .is_provisioned = false,
            .wifi_controller = {},
            .mqtt_controller =
                {
                    .auto_reconnect = true,
                    .running = false,
                },
        },
    .device_name = "SmartRing",
    .mac_address = "",
    .sleep_timeout = 60,
    .order_mode = 'a',
    .connection.type = NULL,
    .flags.has_flags = 0x00,
    .flags =
        {
            .has_flags = 0x00,
        },
    .sensor =
        {
            .full_deposit = 1,
            .no_deposit = 0,
            .new_reading = 0,
            .old_reading = 0,
            .last_sensor_sent = 0,
            .static_counter = 0,
            .calibration =
                {
                    .timestamp = -1,
                    .number_of_readings = 0,
                    .sum_of_readings = 0,
                    .calibrating = false,
                    .step = 0,
                },
        },
};

esp_err_t configure_mac_address(void) {
  char mac[18] = {0};
  const char *TAG = "BASE_MAC";
  // Get the base MAC address from different sources
  uint8_t base_mac_addr[8] = {0};
  esp_err_t ret = ESP_OK;

  // Get base MAC address from EFUSE BLK0(default option)
  ret = esp_efuse_mac_get_default(&base_mac_addr);
  // ret = esp_read_mac(base_mac_addr,ESP_MAC_WIFI_STA);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get base MAC address from EFUSE BLK0. (%s)",
             esp_err_to_name(ret));
    ESP_LOGE(TAG, "Aborting");
    abort();
    return 1;
  }

  ESP_LOGI(TAG, "Base MAC Address read from EFUSE BLK0");
  sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", base_mac_addr[0],
          base_mac_addr[1], base_mac_addr[2], base_mac_addr[3],
          base_mac_addr[4], base_mac_addr[5]);

  ESP_LOGI(TAG, "MAC: %s\n", (char *)mac);

  strcpy(controller.mac_address, mac);

  return ESP_OK;
}

// GETTERS AND SETTERS FOR THE MAIN CONTROLLER

struct smart_ring_controller_t *smart_ring_get_controller() {
  return &controller;
}

char *smart_ring_get_mac_address() { return controller.mac_address; }

void smart_ring_set_device_name(char *device_name) {
  strcpy(controller.device_name, device_name);

  smart_ring_ui_set_device_name(device_name);
}

char *smart_ring_get_device_name() { return controller.device_name; }

void smart_ring_set_user_name(char *user_name) {
  strcpy(controller.user_name, user_name);
}

char *smart_ring_get_user_name() { return controller.user_name; }

void smart_ring_set_user_role(char user_role) {
  controller.user_role = user_role;

  // if (user_role == 'm') {
    controller.ui_controller->is_master = true;
    controller.ui_controller->is_admin = false;
  // } else if (user_role == 'a') {
  //   controller.ui_controller->is_admin = true;
  //   controller.ui_controller->is_master = false;
  // } else {
  //   controller.ui_controller->is_master = false;
  //   controller.ui_controller->is_admin = false;
  // }
}

char smart_ring_get_user_role() { return controller.user_role; }

uint8_t smart_ring_get_stock() { return controller.stock; }

void smart_ring_set_stock(uint8_t stock) {
  controller.stock = stock;
  smart_ring_ui_set_stock(stock);
}

void smart_ring_set_order_mode(char order_mode) {
  controller.order_mode = order_mode;
  smart_ring_ui_set_order_mode(order_mode);
}

char smart_ring_get_order_mode() { return controller.order_mode; }