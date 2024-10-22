/**
 * @file nvs.c
 * @authors  Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the initialization and
 * update of the NVS storage portion of the device
 *
 *
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "libs.h"

// Tag for logging to the monitor
static const char *TAG = "NVS";

//
// Save WIFI credentials to NVS
//
esp_err_t nvs_save_wifi_credentials(char *ssid, char *password, bool provisioned) {

  nvs_handle handle;
  esp_err_t err;

  ESP_LOGI(TAG, "Saving WiFi credentials to flash");

  err = nvs_open("wifi", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"wifi\" space : %s", esp_err_to_name(err));
    return err;
  }

  // Set SSID
  err = nvs_set_str(handle, "ssid", ssid);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"ssid\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  // Set Password
  err = nvs_set_str(handle, "password", password);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"password\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  // Set the device as provisioned
  err = nvs_set_u8(handle, "provisioned", provisioned);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"provisioned\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  // Commit credentials to NVS
  err = nvs_commit(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error commiting \"wifi\" changes : %s",
             esp_err_to_name(err));
    return err;
  }

  nvs_close(handle);

  ESP_LOGI(TAG, "Successfully stored \"wifi\" credentials");
  return ESP_OK;
}

//
// Load WIFI credentials to NVS
//
esp_err_t nvs_load_wifi_credentials(void) {
  nvs_handle handle;
  esp_err_t err;

  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  ESP_LOGI(TAG, "Loading WiFi credentials from flash");

  err = nvs_open("wifi", NVS_READONLY, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"wifi\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  // Load SSID
  size_t required_size;
  err = nvs_get_str(handle, "ssid", NULL, &required_size);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"ssid\" size : %s", esp_err_to_name(err));
    return err;
  }

  char *SavedData = malloc(required_size);
  err = nvs_get_str(handle, "ssid", SavedData, &required_size);
  if (err != ESP_OK) {
    free(SavedData);
    ESP_LOGE(TAG, "Error retrieving \"ssid\" information");
    return err;
  }

  strcpy(&controller->connection.wifi_controller.ssid, SavedData);

  ESP_LOGI(TAG, "Successfully retrieved \"ssid\" information");

  // Load Password
  err = nvs_get_str(handle, "password", NULL, &required_size);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"password\" size : %s",
             esp_err_to_name(err));
    return err;
  }

  free(SavedData);

  SavedData = malloc(required_size);
  err = nvs_get_str(handle, "password", SavedData, &required_size);
  if (err != ESP_OK) {
    free(SavedData);
    ESP_LOGE(TAG, "Error retrieving \"password\" information");
    return err;
  }

  strcpy(&controller->connection.wifi_controller.password, SavedData);

  ESP_LOGI(TAG, "Successfully retrieved \"password\" information");

  // Load if the device is provisioned
  uint8_t provisioned;
  err = nvs_get_u8(handle, "provisioned", &provisioned);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"provisioned\" information");
    return err;
  }

  controller->connection.is_provisioned = provisioned;

  free(SavedData);
  nvs_close(handle);

  return ESP_OK;
}

//
// Clear WIFI credentials to NVS
//
esp_err_t nvs_clear_wifi_credentials() {
  nvs_handle_t handle;
  esp_err_t err;

  ESP_LOGI(TAG, "Clearing WiFi credentials from flash");

  err = nvs_open("wifi", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"wifi\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  err = nvs_erase_all(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error deleting \"wifi\" keys : %s", esp_err_to_name(err));
  }

  // Commit credentials to NVS
  err = nvs_commit(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error commiting \"wifi\" changes : %s",
             esp_err_to_name(err));
    return err;
  }

  nvs_close(handle);

  ESP_LOGI(TAG, "Successfully stored \"wifi\" credentials");
  return ESP_OK;
}

//
// Save calibration values to NVS
//
esp_err_t nvs_save_calibration(int no_deposit, int full_deposit, bool is_stable, int stock) {
  nvs_handle handle;
  esp_err_t err;

  ESP_LOGI(TAG, "Saving configuration values to flash");

  err = nvs_open("configuration", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"configuration\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  // Set no deposit
  err = nvs_set_u32(handle, "no_deposit", no_deposit);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"no_deposit\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  // Set full deposit
  err = nvs_set_u32(handle, "full_deposit", full_deposit);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"full_deposit\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  err = nvs_set_u8(handle, "is_stable", is_stable);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"full_deposit\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  err = nvs_set_u16(handle, "stock", stock);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"stock\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  // Commit credentials to NVS
  err = nvs_commit(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error commiting \"configuration\" changes : %s",
             esp_err_to_name(err));
    return err;
  }

#ifndef NDEBUG
  ESP_LOGI(TAG,
           "SAVED calibration values\n\tNO_DEPOSIT : %d\n\tFULL "
           "DEPOSIT : %d\n\tSTOCK : %d\n\t",
           no_deposit, full_deposit, stock);
#endif

  nvs_close(handle);

  ESP_LOGI(TAG, "Successfully stored \"configuration\" credentials");
  return ESP_OK;
}

//
// Load calibration values to NVS
//
esp_err_t nvs_load_calibration() {
  nvs_handle handle;
  esp_err_t err;

  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  ESP_LOGI(TAG, "Loading calibration values to flash");

  err = nvs_open("configuration", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"configuration\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  // Load no_deposit
  uint32_t no_deposit;
  err = nvs_get_u32(handle, "no_deposit", &no_deposit);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"no_deposit\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  controller->sensor.no_deposit = no_deposit;

  // Load full_deposit
  uint32_t full_deposit;
  err = nvs_get_u32(handle, "full_deposit", &full_deposit);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"full_deposit\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  controller->sensor.full_deposit = full_deposit;

  // Load is_stable
  uint32_t is_stable;
  err = nvs_get_u8(handle, "is_stable", &is_stable);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"is_stable\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  controller->sensor.stable = is_stable;

  // Load stock
  uint16_t stock;
  err = nvs_get_u16(handle, "stock", &stock);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"stock\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  controller->stock = stock;

#ifndef NDEBUG
  ESP_LOGI(TAG,
           "Loaded calibration values\n\tNO_DEPOSIT : %d\n\tFULL "
           "DEPOSIT : %d\n\tSTOCK : %d\n\t",
           controller->sensor.no_deposit, controller->sensor.full_deposit,
           controller->stock);
#endif

  nvs_close(handle);

  return ESP_OK;
}

//
// Save order mode to NVS
//
esp_err_t nvs_save_order_mode(char order_mode) {
  nvs_handle handle;
  esp_err_t err;

  ESP_LOGI(TAG, "Saving order mode to flash");

  err = nvs_open("configuration", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"configuration\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  // Set order mode
  err = nvs_set_u8(handle, "order_mode", order_mode);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"order_mode\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  // Commit credentials to NVS
  err = nvs_commit(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error commiting \"configuration\" changes : %s",
             esp_err_to_name(err));
    return err;
  }

  nvs_close(handle);

  ESP_LOGI(TAG, "Successfully stored \"configuration\" credentials");
  return ESP_OK;
}

//
// Load order mode to NVS
//
esp_err_t nvs_load_order_mode() {
  nvs_handle handle;
  esp_err_t err;

  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  ESP_LOGI(TAG, "Loading order mode value from flash");

  err = nvs_open("configuration", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"configuration\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  // Load order mode
  uint8_t order_mode;
  err = nvs_get_u8(handle, "order_mode", &order_mode);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"order_mode\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  controller->order_mode = (char)order_mode;

  nvs_close(handle);

  return ESP_OK;
}

//
// Save connection type to NVS
//
esp_err_t nvs_save_connection_type(char connection_type) {
  nvs_handle handle;
  esp_err_t err;

  ESP_LOGI(TAG, "Saving connection type to flash");

  err = nvs_open("configuration", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"configuration\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  // Set order mode
  err = nvs_set_u8(handle, "connection_type", connection_type);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"connection_type\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  // Commit credentials to NVS
  err = nvs_commit(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error commiting \"configuration\" changes : %s",
             esp_err_to_name(err));
    return err;
  }

  nvs_close(handle);

  ESP_LOGI(TAG, "Successfully stored \"configuration\" credentials");
  return ESP_OK;
}

//
// Load connection type to NVS
//
esp_err_t nvs_load_connection_type() {
  nvs_handle handle;
  esp_err_t err;

  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  ESP_LOGI(TAG, "Loading connection type value from flash");

  err = nvs_open("configuration", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"configuration\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  // Load order mode
  uint8_t connection_type;
  err = nvs_get_u8(handle, "connection_type", &connection_type);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"connection_type\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  controller->connection.type = (char)connection_type;

  nvs_close(handle);

  return ESP_OK;
}

//
// Load temporary mqtt certificates to NVS
//
esp_err_t nvs_load_temporary_mqtt_certificate_pem(char* pem) {
  nvs_handle handle;
  esp_err_t err;

  ESP_LOGI(TAG, "Load MQTT \"cert_pem\" into buffer");

  err = nvs_open("mqtt", NVS_READONLY, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"mqtt\" space : %s", esp_err_to_name(err));
    return err;
  }

  // Load mqtt certificates pem crt start
  size_t required_size;
  err = nvs_get_str(handle, "cert_pem", NULL, &required_size);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error retrieving \"cert_pem\" size : %s", esp_err_to_name(err));
    return err;
  }

  char *SavedData = malloc(required_size);
  err = nvs_get_str(handle, "cert_pem", SavedData, &required_size);
  if (err != ESP_OK) {
    free(SavedData);
    ESP_LOGE(TAG, "Error retrieving \"cert_pem\" information");
    return err;
  }

  strcpy(pem, SavedData);

  ESP_LOGI(TAG, "Successfully retrieved \"cert_pem\" information");

  free(SavedData);
  nvs_close(handle);

  return ESP_OK;
}

//
// Save mqtt certificates to NVS
//
esp_err_t nvs_save_mqtt_certificate_pem(char *pem) {

  nvs_handle handle;
  esp_err_t err;

  ESP_LOGI(TAG, "Saving certificate pem to flash");

  err = nvs_open("mqtt", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"mqtt\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  // Set certificate PEM
  err = nvs_set_str(handle, "cert_pem", pem);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error storing \"cert_pem\" information : %s",
             esp_err_to_name(err));
    return err;
  }

  // Commit credentials to NVS
  err = nvs_commit(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error commiting \"cert_pem\" changes : %s",
             esp_err_to_name(err));
    return err;
  }

  nvs_close(handle);

  ESP_LOGI(TAG, "Successfully stored \"mqtt certificate pem\" credentials");
  return ESP_OK;
}

//
// Clear temporary mqtt certificates to NVS
//
esp_err_t nvs_clear_certificate_credentials(void) {
  nvs_handle_t handle;
  esp_err_t err;
 
  ESP_LOGI(TAG, "Clearing MQTT credentials from flash");

  err = nvs_open("mqtt", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening nvs \"mqtt\" space : %s",
             esp_err_to_name(err));
    return err;
  }

  err = nvs_erase_all(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error deleting \"mqtt\" keys : %s", esp_err_to_name(err));
  }

  // Commit credentials to NVS
  err = nvs_commit(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error commiting \"mqtt\" changes : %s",
             esp_err_to_name(err));
    return err;
  }

  nvs_close(handle);

  ESP_LOGI(TAG, "Successfully stored \"mqtt\" credentials");
  return ESP_OK;
}
