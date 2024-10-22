/**
 * @file http_client.c
 * @authors  Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the FOTA update of the
 * device and the HTTP communication with the AWS.
 *
 *
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "http_client.h"
#include "libs.h"

static const char *TAG = "HTTP_CLIENT";

static bool connection_error        = false;
static bool connection_has_response = false;
static int  connection_retries      = 0;


/**
 * @brief
 * Object structure to store the information about the ongoing device firmware
 * update.
 *
 */
struct smart_ring_http_client_ota_information {
  /**
   * @brief
   * Current number of data bytes processed
   *
   */
  int current_position;

  /**
   * @brief
   * Last percentage saved and showed on the device UI.
   *
   */
  int last_percentage;

  /**
   * @brief
   * ESP-IDF FOTA handle
   *
   */
  esp_ota_handle_t ota_handle;

  /**
   * @brief
   * ESP-IDF FOTA partition
   *
   */
  esp_partition_t *ota_partition;
};

/**
 * @brief
 * Object to process ongoing firmware updates
 *
 */
struct smart_ring_http_client_ota_information ota_information = {
    .current_position = 0,
    .last_percentage = 0,

};

/**
 * @brief
 * Handle the responses from the AWS and trigger the corresponding callbacks
 *
 * @param event {esp_http_client_event_t} - Event containing data and client
 * handle
 * @return esp_err_t - Should return ESP-OK to continue correct functioning
 */
esp_err_t smart_ring_http_client_event_handler(esp_http_client_event_t *event) {

  static char *event_data;
  static int event_data_length;

  switch (event->event_id) {
  case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;

  case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

  case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;

  case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", event->header_key, event->header_value);
            break;

  case HTTP_EVENT_ON_DATA:
           ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", event->data_len);

           if (!esp_http_client_is_chunked_response(event->client)) {
               char *char_temporary = NULL;

              esp_http_client_get_header(event->client, "X-EVT-TYPE", &char_temporary);
              int content_length = esp_http_client_get_content_length(event->client);
              int req_err_code   = esp_http_client_get_status_code(event->client);

              ESP_LOGI(TAG, "Content length %d", content_length);

              if (atoi(char_temporary) == HTTP_CLIENT_GET_CHECK_FIRMWARE) {
              ESP_LOGI(TAG, "FIRMWARE ERR CODE: %d", req_err_code);
              if (req_err_code == 200) {
                  ESP_LOGI(TAG, "Firmware available");
              } else {
                 ESP_LOGI(TAG, "No new firmware available");
              }

              esp_http_client_close(event->client);
              return ESP_OK;
          } 
          else {
              event_data_length = event->data_len;
              event_data        = event->data;
              ota_information.current_position += event_data_length;
              int current_percentage = ota_information.current_position * 100 / content_length;
#ifndef NDEBUG
              ESP_LOGI(TAG, "%d - %d - %d\n\n\tOTA AT %d\n", event_data_length, ota_information.current_position, 
                                                             content_length, current_percentage);
#endif
              if (current_percentage > ota_information.last_percentage + 5) {
                  ota_information.last_percentage = current_percentage;
                  smart_ring_ui_update_change_percentage(ota_information.last_percentage);
                  vTaskDelay(100 / portTICK_PERIOD_MS);
              }
              esp_err_t ota_err = esp_ota_write(ota_information.ota_handle, (const char *)event->data, event->data_len);
              if (ota_err != ESP_OK) {
                  ESP_LOGE(TAG, "Writing to partition failed (%s)", esp_err_to_name(ota_err));
                  esp_ota_abort(ota_information.ota_handle);
                  break;
              }
          }
          event_data_length += event->data_len;
         }
         break;

  case HTTP_EVENT_ON_FINISH:
           ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
           if (esp_http_client_get_status_code(event->client) == 200 && smart_ring_get_controller()->flags.flag.update_failed) {
#ifndef NDEBUG
             ESP_LOGI(TAG, "Ota canceled, closed connection");
#endif
           } 
           else {
              if (event->data != NULL) {
                  int request_status_code = esp_http_client_get_status_code(event->client);
                 free(event->data);
                 event->data = NULL;
              }
              event->data_len = 0;
         }
         break;

  case HTTP_EVENT_DISCONNECTED:
           ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
           int mbedtls_err = 0;
           esp_err_t err   = esp_tls_get_and_clear_last_error(event->data, &mbedtls_err, NULL);
           if (err != 0) {
               ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
               ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
           }
           if (event_data != NULL) {
               free(event_data);
               event_data = NULL;
           }
           event_data_length = 0;
           break;

  default:
    break;
  }

  return ESP_OK;
}

// Check if there is a new firmware (outdated)
void smart_ring_http_client_get_check_firmware() {

  esp_err_t err_http;

  char type[sizeof(int) + 1];
  itoa(HTTP_CLIENT_GET_CHECK_FIRMWARE, type, 10);

  char url[120] = {0};

  sprintf(url, "%s/%s?sr=true&version=%s", API_ENDPOINT, "firmware",
          FIRMWARE_VERSION);

  esp_http_client_config_t config = {
      .url = url,
      .event_handler = smart_ring_http_client_event_handler,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .method = HTTP_METHOD_GET,
  };

  ESP_LOGI(TAG, "URL: %s", url);
  
  esp_http_client_handle_t client = esp_http_client_init(&config);
  // Add api key header to validate on API
  esp_http_client_set_header(client, "X-Api-Key", API_KEY);
  
  esp_http_client_set_header(client, "X-EVT-TYPE", type);

  int status_code = 500;

  do {
    err_http = esp_http_client_perform(client);

    status_code = esp_http_client_get_status_code(client);
    ESP_LOGI(TAG, "Error check update %d", status_code);

    if (err_http == ESP_OK) {
      ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %lld", status_code,
               (long long)esp_http_client_get_content_length(client));
      // TODO : erase error flag
    } else {
      ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err_http));
      // TODO : Add error flag and increase the retries
      connection_error = true;
      connection_retries++;
    }
  } while (connection_error && connection_retries < 3 && status_code > 400);

  connection_error = false;
  connection_has_response = false;
  connection_retries = 0;
  esp_http_client_cleanup(client);

  // TODO : Error on connection set the error flag for the system
  switch (status_code) {
  case 200:
    smart_ring_http_client_get_update_firmware();
    break;
  case 400:
    smart_ring_ui_update_state(STATE_5);
    break;

  default:
    break;
  }
}

// Update the firmware to the latest
void smart_ring_http_client_get_update_firmware() {

  esp_err_t err_http;

  ESP_LOGI(TAG, "Beginning OTA");
  ota_information.ota_partition = esp_ota_get_next_update_partition(NULL);
  ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x",
           ota_information.ota_partition->subtype,
           ota_information.ota_partition->address);
  assert(ota_information.ota_partition != NULL);
  err_http = esp_ota_begin(ota_information.ota_partition, OTA_SIZE_UNKNOWN,
                           &ota_information.ota_handle);

  if (ESP_OK != err_http) {
    ESP_LOGE(TAG, "Error starting ota handle and partition : %s",
             esp_err_to_name(err_http));
    // TODO : Set the error flag
    return;
  }

  ESP_LOGI(TAG, "OTA Handle started\nRequesting image");

  smart_ring_ui_update_download_started();

  char type[sizeof(int) + 1];
  itoa(HTTP_CLIENT_GET_UPDATE_FIRMWARE, type, 10);

  char url[120] = {0};

  sprintf(url, "%s/%s?sr=true&version=%s", API_ENDPOINT, "firmware",
          FIRMWARE_VERSION);
  esp_http_client_config_t config = {
      .url = API_ENDPOINT,
      .event_handler = smart_ring_http_client_event_handler,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .method = HTTP_METHOD_GET,
  };
  

  ESP_LOGI(TAG, "URL: %s", url);

  esp_http_client_handle_t client = esp_http_client_init(&config);


  // Add api key header to validate on API
  esp_http_client_set_header(client, "X-Api-Key", API_KEY);
  esp_http_client_set_header(client, "X-EVT-TYPE", type);

  esp_http_client_set_url(client, url);

  do {
    err_http = esp_http_client_perform(client);

    if (err_http == ESP_OK) {
      ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %lld",
               esp_http_client_get_status_code(client),
               (long long)esp_http_client_get_content_length(client));

    } else {
      ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err_http));
      connection_error = true;
      connection_retries++;
    }
  } while (connection_retries < 3 && connection_error &&
           esp_http_client_get_status_code(client) != 200);

  int status_code = esp_http_client_get_status_code(client);

  connection_error = false;
  connection_has_response = false;
  connection_retries = 0;

  if (connection_error) {

    smart_ring_get_controller()->flags.flag.update_failed = true;

  } else {
    err_http = esp_ota_end(ota_information.ota_handle);
    if (ESP_OK != err_http) {
      if (err_http == ESP_ERR_OTA_VALIDATE_FAILED) {
        ESP_LOGE(TAG, "Image validation failed, image is corrupted");
      } else {
        ESP_LOGE(TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err_http));
      }

      esp_ota_abort(ota_information.ota_handle);
      smart_ring_get_controller()->flags.flag.update_failed = true;
    } else {
      err_http = esp_ota_set_boot_partition(ota_information.ota_partition);
      if (err_http != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!",
                 esp_err_to_name(err_http));
        smart_ring_get_controller()->flags.flag.update_failed = true;
      } else {
        smart_ring_get_controller()->flags.flag.update_complete = true;
      }
    }
  }
  esp_http_client_cleanup(client);
}

esp_err_t certificate_http_event_handle(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read

    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            
            break;
        case HTTP_EVENT_ON_DATA:
             ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            if (!esp_http_client_is_chunked_response(evt->client)) {
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } 
                else {
                    const int buffer_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(buffer_len);
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (buffer_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
             ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
             if(evt->data != NULL) {
                free(evt->data);
                evt->data = NULL;
                evt->data_len = 0;
             }
             if(output_buffer != NULL) {
                printf("%s\n", (char*) output_buffer);
                   
                // Parse to JSON
                const cJSON *item = NULL;

                const cJSON *certificate_json = cJSON_Parse((char*) output_buffer);
                free(output_buffer);
                output_buffer = NULL;

                if (certificate_json == NULL) {
                    const char *error_ptr = cJSON_GetErrorPtr();
                    if (error_ptr != NULL) {
                        ESP_LOGE(TAG, "Error parsing JSON certificate object : %s", error_ptr);
                    }
                }

                // Get information from JSON object
                // Get Http Status Code
                int httpStatusCode  = 500;
            
                item = cJSON_GetObjectItemCaseSensitive(certificate_json, "createKeysResult");
                if (!cJSON_IsObject(item)) {
                    ESP_LOGE(TAG, "Information is not a object");
                }
                else {

                    int number_of_items = cJSON_GetArraySize(item);
                    ESP_LOGI(TAG, " Received %d createKeysResult", number_of_items);

                    const cJSON *createKeysResult = cJSON_GetObjectItemCaseSensitive(certificate_json,"createKeysResult");
                    const cJSON *metadata         = cJSON_GetObjectItemCaseSensitive(createKeysResult,"$metadata"); 

                    httpStatusCode = cJSON_GetObjectItem(metadata, "httpStatusCode")->valueint;
                    printf("HTTP Status code %d\n", httpStatusCode);

                    if(httpStatusCode == 200) {
                
                       // Get Device Certificate and Store in NVS
                       const char *certificatePem = cJSON_GetObjectItemCaseSensitive(createKeysResult, "certificatePem")->valuestring;
                       if(certificatePem != NULL) {
                          nvs_save_mqtt_certificate_pem(certificatePem);
                          ESP_LOGI(TAG, "certificatePem: %s\n",  certificatePem);
                       }
                       else 
                          ESP_LOGE(TAG, "certificatePem is not a string \n");

                       // Get Private key and Store in SPIFFS
                       const cJSON *keyPair   = cJSON_GetObjectItemCaseSensitive(createKeysResult,"keyPair"); 
                       const char *privateKey = cJSON_GetObjectItemCaseSensitive(keyPair, "PrivateKey")->valuestring;
                       if(privateKey != NULL) {
                          spiffs.saveMQTTCertificatePrivateKey(privateKey);
                          ESP_LOGI(TAG, "PrivateKey  %s\n", privateKey);
                       }
                       else 
                          ESP_LOGE(TAG, "PrivateKey is not a string\n");
                   }
                }
             }

             output_len = 0;
             break;
        case HTTP_EVENT_DISCONNECTED:
             ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
             int mbedtls_err = 0;
             esp_err_t err   =
             esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
             if (err != 0) {
                 ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                 ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
             }
             if (output_buffer != NULL) {
                 free(output_buffer);
                 output_buffer = NULL;
             }
             output_len = 0;
             break;
    }
    return ESP_OK;
}


void smart_ring_http_client_get_certificate(void) {
  
     esp_err_t err_http;
  
     const esp_http_client_config_t config = {
      .url               = MQTT_API_ENDPOINT,
      .event_handler     = certificate_http_event_handle,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .transport_type    = HTTP_TRANSPORT_OVER_SSL,
      .method            = HTTP_METHOD_GET,
     };
     
  
     ESP_LOGI(TAG, "URL: %s", MQTT_API_ENDPOINT);

     esp_http_client_handle_t client = esp_http_client_init(&config);

     // Add headers to validate on API
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "x-api-key", MQTT_API_KEY);
    esp_http_client_set_header(client, "Content-Type", "application/json");
  
    char* post_data = calloc(100, sizeof(char));
    sprintf(post_data, "{\"SerialNumber\":\"%s\"}", smart_ring_get_mac_address());
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    err_http    = esp_http_client_perform(client);
    ESP_LOGI(TAG, "POST DATA: %s", post_data);
    free(post_data);

  
    esp_http_client_set_method(client, HTTP_METHOD_GET);

    int status_code = esp_http_client_get_status_code(client);

    if (err_http == ESP_OK) 
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %lld", status_code, (long long)esp_http_client_get_content_length(client));
    else 
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err_http));
            

    while(esp_http_client_cleanup(client)!=ESP_OK) {
        ESP_LOGE(TAG,"http cleanup failed!");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG,"http connection closed clean");
}


/* END OF FILE */