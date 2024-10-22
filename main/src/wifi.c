/**
 * @file wifi.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the configuration and
 * control of the wifi interface
 *
 *
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "libs.h"

// Tag used for logging purposes
static const char *TAG = "WIFI";

// Queue handle used to manipulate the main queue of events
static QueueHandle_t wifi_app_queue_handle;

// Global wifi group
static volatile EventGroupHandle_t wifi_app_event_group;

// Variable for the retries
static int retries = 0;

/**
 * @brief  Wifi event handler for status changes on the interface and connection
 * @note   It may change during the course of the firmware integration
 * @param  *arg: User custom argument to be passed if required
 * @param  event_base: Group of event type - WiFi in this case
 * @param  event_id: Type of event
 * @param  *event_data: Data triggered by the event, to be used if necessary
 * @retval None
 */
static void wifi_app_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT) {
    switch (event_id) {
    case WIFI_EVENT_AP_START:
      ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
      int err = http_server_start();
      if (err != ESP_OK) {
        ESP_LOGI(TAG, "ERROR STARTING SERVER: %s", esp_err_to_name(err));
      }
      break;
    case WIFI_EVENT_AP_STOP:
      ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
      http_server_stop();
      break;
    case WIFI_EVENT_AP_STACONNECTED:
      ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
      break;
    case WIFI_EVENT_AP_STADISCONNECTED:
      ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
      break;
    case WIFI_EVENT_STA_START:
      ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
      break;
    case WIFI_EVENT_STA_CONNECTED:
      ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
      break;
    case WIFI_EVENT_STA_DISCONNECTED:
      ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");

      if (smart_ring_get_controller()->connection.is_connected) {
        // Change the signal icon to no network
        smart_ring_ui_set_rssi_level(0);
      }

      smart_ring_get_controller()->connection.is_connected = false;

      wifi_event_sta_disconnected_t *wifi_event_sta_disconnected =
          (wifi_event_sta_disconnected_t *)malloc(
              sizeof(wifi_event_sta_disconnected_t));
      *wifi_event_sta_disconnected =
          *((wifi_event_sta_disconnected_t *)event_data);
      printf("WIFI_EVENT_STA_DISCONNECTED, reason code %d\n",
             wifi_event_sta_disconnected->reason);

      if (!smart_ring_get_controller()->connection.is_provisioned) {
        provision_set_status(PROVISION_FAILED);
        break;
      } else {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
#ifndef NDEBUG
        ESP_LOGI(TAG, "DISCONNECTED, Retrying to connect\n");
#endif

        if (smart_ring_ui_get_controller()->state == STATE_4 && retries >= 2) {
          boot_update_warning_label("Could not connect");
          vTaskDelay(3000 / portTICK_PERIOD_MS);
          smart_ring_ui_update_state(STATE_5);
        }

        if (smart_ring_ui_get_controller()->state > STATE_6) {
          smart_ring_ui_update_state(STATE_5);
          vTaskDelay(500 / portTICK_PERIOD_MS);
          smart_ring_ui_update_state(STATE_6);
        }

        retries++;
        esp_wifi_connect();
      }
      free(wifi_event_sta_disconnected);
      break;
    }
  } else if (event_base == IP_EVENT) {
    switch (event_id) {
    case IP_EVENT_STA_GOT_IP:
      ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");

      wifi_config_t current_config = {};
      esp_wifi_get_config(WIFI_IF_STA, &current_config);

      if (!smart_ring_get_controller()->connection.is_provisioned) {
        provision_set_status(PROVISION_SUCCESSFUL);

#ifndef NDEBUG
        ESP_LOGI(TAG, "Saving credentials\nSSID : %s\nPASS : %s",
                 (char *)&current_config.sta.ssid,
                 (char *)&current_config.sta.password);
#endif

        nvs_save_wifi_credentials(&current_config.sta.ssid,
                                  &current_config.sta.password, true);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_restart();
      }

      // Set rssi level
      wifi_get_rssi();

      // Save the connected flag and reset retries variable
      smart_ring_get_controller()->connection.is_connected = true;

      retries = 0;

      break;
    }
  }
}

/**
 * @brief  Configure the event handler function for the interface
 * @retval None
 */
static void wifi_app_event_handler_init(void) {
  // Event loop for the WiFi driver
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Event handler for the connection
  esp_event_handler_instance_t instance_wifi_event;
  esp_event_handler_instance_t instance_ip_event;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL,
      &instance_wifi_event));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL,
      &instance_ip_event));
}

void wifi_interface_start(
    struct smart_ring_connection_controller_t *connection_controller) {
  ESP_LOGI(TAG, "STARTING WIFI INTERFACE");

  // Create Wifi application event group
  wifi_app_event_group = xEventGroupCreate();

  // Start the WiFi application task
  EventBits_t eventBits;

  // Initialize the event handler
  wifi_app_event_handler_init();

  // Initialize the TCP/IP stack and WiFi config
  // Initialize the TCP stack
  ESP_ERROR_CHECK(esp_netif_init());

  // Default WiFi config - operations must be in this order!
  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  int err = esp_wifi_init(&wifi_init_config);
  if (err != ESP_OK) {
    printf("ERROR %s\n", esp_err_to_name(err));
  }
  esp_netif_create_default_wifi_sta();
  connection_controller->wifi_controller.esp_netif_ap =
      esp_netif_create_default_wifi_ap();

  ESP_LOGI(TAG, "Event handler and Wifi initiated");

  ESP_ERROR_CHECK(esp_wifi_start());

  // Define AP config if device is not provisioned
  if (!connection_controller->is_provisioned) {
    wifi_ap_init(connection_controller);
  } else {
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_LOGI(TAG, "WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS");

    wifi_config_t sta_config = {};

    strcpy(&sta_config.sta.ssid, &connection_controller->wifi_controller.ssid);
    strcpy(&sta_config.sta.password,
           &connection_controller->wifi_controller.password);

#ifndef NDEBUG
    ESP_LOGI(TAG, "Using saved credentials\nSSID : %s\nPASS : %s",
             (char *)&sta_config.sta.ssid, (char *)&sta_config.ap.password);
#endif
    esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config);
    esp_wifi_connect();
  }
}

static void
wifi_ap_init(struct smart_ring_connection_controller_t *connection_controller) {
  ESP_LOGI(TAG, "CONFIGURING AP INTERFACE");

  char ap_ssid[9] = {0};

  char *full_mac = smart_ring_get_mac_address();

  sprintf(ap_ssid, "SR_%c%c%c%c", full_mac[12], full_mac[13], full_mac[15],
          full_mac[16]);

  printf("AP SSID: %s\n", ap_ssid);

  // SoftAP - WiFi access point configuration

  wifi_config_t ap_config = {
      .ap =
          {
              .password = WIFI_AP_PASSWORD,
              .channel = WIFI_AP_CHANNEL,
              .ssid_hidden = WIFI_AP_SSID_HIDDEN,
              .authmode = WIFI_AUTH_WPA2_PSK,
              .max_connection = WIFI_AP_MAX_CONNECTIONS,
              .beacon_interval = WIFI_AP_BEACON_INTERVAL,
          },
  };

  strcpy(&ap_config.ap.ssid, ap_ssid);

  // Configure DHCP for the AP
  esp_netif_ip_info_t ap_ip_info;
  memset(&ap_ip_info, 0x00, sizeof(ap_ip_info));

  esp_netif_dhcps_stop(connection_controller->wifi_controller
                           .esp_netif_ap); ///>must call this first
  inet_pton(
      AF_INET, WIFI_AP_IP,
      &ap_ip_info.ip); ///> Assign access point's static IP , GW, and netmask
  inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
  inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);
  ESP_ERROR_CHECK(esp_netif_set_ip_info(
      connection_controller->wifi_controller.esp_netif_ap,
      &ap_ip_info)); ///>Statically configure the network interface
  ESP_ERROR_CHECK(
      esp_netif_dhcps_start(connection_controller->wifi_controller
                                .esp_netif_ap)); ///>Start the AP DHCP server
                                                 ///(for connecting stations
                                                 /// e.g. your mobile device)

  ESP_ERROR_CHECK(esp_wifi_set_mode(
      WIFI_MODE_APSTA)); ///>Setting the mode as Access Point/ Station Mode
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP,
                                      &ap_config)); ///>Set our configuration
  ESP_ERROR_CHECK(esp_wifi_set_bandwidth(
      WIFI_IF_AP, WIFI_AP_BANDWIDTH)); ///> Our default bandwidth 20MHz
  ESP_ERROR_CHECK(
      esp_wifi_set_ps(WIFI_STA_POWER_SAVE)); ///> Power save set to "NONE"
}

int8_t wifi_get_rssi() {
  wifi_ap_record_t ap_record;

  ESP_LOGI(TAG, "Getting WiFi RSSI");

  esp_wifi_sta_get_ap_info(&ap_record);

#ifndef NDEBUG
  ESP_LOGI(TAG, "RSSI %d", ap_record.rssi);
#endif

  // Set the ui new level
  if (ap_record.rssi > -50) {
    smart_ring_ui_set_rssi_level(3);
  } else if (ap_record.rssi > -70) {
    smart_ring_ui_set_rssi_level(2);
  } else if (ap_record.rssi > -90) {
    smart_ring_ui_set_rssi_level(1);
  } else {
    smart_ring_ui_set_rssi_level(0);
  }

  return ap_record.rssi;
}

void wifi_reset_device() {

  esp_err_t err = nvs_clear_wifi_credentials();

  if (err == ESP_OK) {
    esp_restart();
  }

  ESP_LOGE(TAG, "ERROR DELETING CREDENTIALS");
  // TODO : Change screen to error
}