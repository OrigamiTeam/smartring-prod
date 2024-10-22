/**
 * @file http_server.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the creation and handling of
 * the HTTP server for the device provisioning
 *
 *
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "libs.h"

// Tag used for ESP serial console messages
static const char *TAG = "http_server";

// HTTP server task handle
static httpd_handle_t http_server_handle = NULL;

// HTTP server monitor task handle
static TaskHandle_t task_http_server_monitor = NULL;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t http_server_monitor_queue_handle;

// Variable with the provision status
static enum smart_ring_provision_status_t provision_status;

// Embedded files: JQuery, index.html, app.css, app.js and favicon.ico files
/// Start of JQUERY file
extern const uint8_t jquery_3_6_0_min_js_start[] asm("_binary_jquery_3_6_0_min_js_start");

/// End of JQUERY file
extern const uint8_t jquery_3_6_0_min_js_end[] asm("_binary_jquery_3_6_0_min_js_end");
/// Start of HTML file
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
/// End of HTML file
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
/// Start of CSS file
extern const uint8_t index_css_start[] asm("_binary_index_css_start");
/// End of CSS file
extern const uint8_t index_css_end[] asm("_binary_index_css_end");
/// Start of JS file
extern const uint8_t app_js_start[] asm("_binary_app_js_start");
/// End of JS file
extern const uint8_t app_js_end[] asm("_binary_app_js_end");
/// Start of QL image
extern const uint8_t quantum_leap_png_start[] asm("_binary_quantum_leap_png_start");
/// End of QL image
extern const uint8_t quantum_leap_png_end[] asm("_binary_quantum_leap_png_end");
/// Start of Fonte Viva image
extern const uint8_t fonte_viva_png_start[] asm("_binary_fonte_viva_png_start");
/// End of Fonte Viva image
extern const uint8_t fonte_viva_png_end[] asm("_binary_fonte_viva_png_end");

static esp_err_t http_server_jquery_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "Jquery requested");

  httpd_resp_set_type(req, "application/javascript");
  httpd_resp_send(req, (const char *)jquery_3_6_0_min_js_start, jquery_3_6_0_min_js_end - jquery_3_6_0_min_js_start);
  return ESP_OK;
}

static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "index.html requested");

  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
  return ESP_OK;
}

static esp_err_t http_server_index_css_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "index.css requested");

  httpd_resp_set_type(req, "text/css");
  httpd_resp_send(req, (const char *)index_css_start, index_css_end - index_css_start);
  return ESP_OK;
}

static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "app.js requested");

  httpd_resp_set_type(req, "application/javascript");
  httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);
  return ESP_OK;
}

static esp_err_t http_server_ql_logo_png_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "QL logo requested");

  httpd_resp_set_type(req, "image/png");
  httpd_resp_send(req, (const char *)quantum_leap_png_start, quantum_leap_png_end - quantum_leap_png_start);
  return ESP_OK;
}

static esp_err_t http_server_fv_logo_png_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "FV logo requested");

  httpd_resp_set_type(req, "image/png");
  httpd_resp_send(req, (const char *)fonte_viva_png_start, fonte_viva_png_end - fonte_viva_png_start);
  return ESP_OK;
}

static esp_err_t http_server_scan_networks_handler(httpd_req_t *req)
{
  ESP_LOGI(TAG, "Surrounding networks requested");

  // Scan networks
  const wifi_scan_config_t scan_config = {};
  int err = esp_wifi_scan_start(&scan_config, true);
  ESP_LOGI(TAG, "NETWORKS SCANNED: %s", esp_err_to_name(err));

  // List networks
  uint16_t n_networks;
  err = esp_wifi_scan_get_ap_num(&n_networks);
  ESP_LOGI(TAG, "NETWORKS FOUND: %s - %d", esp_err_to_name(err), n_networks);

  if (n_networks > 10)
  {
    n_networks = 10;
  }

  char response[(n_networks * 33) + (n_networks * 3) + 2];
  strcpy(response, "[\"");

  wifi_ap_record_t networks[n_networks];
  err = esp_wifi_scan_get_ap_records(&n_networks, networks);
  ESP_LOGI(TAG, "NETWORKS RETRIEVED: %s", esp_err_to_name(err));
  
  if (err == ESP_OK)
  {
    ESP_LOGI(TAG, "NETWORKS: %d", n_networks);
    for (size_t i = 0; i < n_networks; i++)
    {
      ESP_LOGI(TAG, "%d - %s", i + 1, networks[i].ssid);
      char temp[33 + 3];
      if (i == n_networks - 1)
      {
        sprintf(temp, "%s\"]", networks[i].ssid);
        strcat(response, temp);
      }
      else
      {
        sprintf(temp, "%s\",\"", networks[i].ssid);
        strcat(response, temp);
      }
    }

    ESP_LOGI(TAG, "Response: %s", response);
  }

  esp_wifi_scan_stop();

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

  return ESP_OK;
}

static esp_err_t http_server_start_provision_handler(httpd_req_t *req)
{

  ESP_LOGI(TAG, "PROVISION REQUESTED");
  // Get the wifi interface
  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  // Save the credentials to the wifi interface config
  size_t len_ssid = 0, len_pass = 0;
  char *ssid_str = NULL, *pass_str = NULL;

  // Get SSID header
  len_ssid = httpd_req_get_hdr_value_len(req, "ssid") + 1;
  if (len_ssid > 1)
  {
    ssid_str = malloc(len_ssid);
    if (httpd_req_get_hdr_value_str(req, "ssid", ssid_str, len_ssid) == ESP_OK)
    {
      ESP_LOGI(TAG, "PROVISION: Found header => ssid: %s", ssid_str);
    }
  }

  // Get Password header
  len_pass = httpd_req_get_hdr_value_len(req, "pass") + 1;
  if (len_pass > 1)
  {
    pass_str = malloc(len_pass);
    if (httpd_req_get_hdr_value_str(req, "pass", pass_str, len_pass) == ESP_OK)
    {
      ESP_LOGI(TAG, "PROVISION: Found header => pass: %s", pass_str);
    }
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, "{\"result\":\"OK\"}", HTTPD_RESP_USE_STRLEN);

  wifi_config_t sta_config = {};

  strcpy(&sta_config.sta.ssid, ssid_str);
  strcpy(&sta_config.sta.password, pass_str);

  int err = esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config);
#ifndef NDEBUG
  ESP_LOGI(TAG, "SET CONFIG: %s", esp_err_to_name(err));
#endif

  err = esp_wifi_connect();
#ifndef NDEBUG
  ESP_LOGI(TAG, "CONNECTION RESULT: %s", esp_err_to_name(err));
#endif
  provision_set_status(PROVISION_CONNECTING);

  free(ssid_str);
  free(pass_str);

  return ESP_OK;
}

static esp_err_t http_server_status_handler(httpd_req_t *req)
{

  char response[14];
  sprintf(response, "{\"status\":%d}", provision_get_status());

  ESP_LOGI(TAG, "PROVISION STATUS: %s", response);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

  return ESP_OK;
}

static esp_err_t http_server_configure(void)
{

  // Generate the default configuration
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  // The core that the HTTP server will run on
  config.core_id = HTTP_SERVER_TASK_CORE_ID;

  // Adjust the default priority to 1 less than the wifi application task
  config.task_priority = HTTP_SERVER_TASK_PRIORITY;

  // Bump up the stack size (default is 4096)
  config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

  // Increase uri handlers
  config.max_uri_handlers = 20;

  // Increase the timeout limits
  config.recv_wait_timeout = 10;
  config.send_wait_timeout = 10;

  ESP_LOGI(TAG,
           "http_server_configure: Starting server on port: '%d' with task "
           "priority: '%d'",
           config.server_port, config.task_priority);

  // Start the httpd server
  if (httpd_start(&http_server_handle, &config) == ESP_OK)
  {
    ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");

    // register query handler
    httpd_uri_t jquery_js = {.uri = "/jquery-3.6.0.min.js",
                             .method = HTTP_GET,
                             .handler = http_server_jquery_handler,
                             .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &jquery_js);

    // register index.html handler
    httpd_uri_t index_html = {.uri = "/",
                              .method = HTTP_GET,
                              .handler = http_server_index_html_handler,
                              .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &index_html);

    // register app.css handler
    httpd_uri_t app_css = {.uri = "/index.css",
                           .method = HTTP_GET,
                           .handler = http_server_index_css_handler,
                           .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &app_css);

    // register app.js handler
    httpd_uri_t app_js = {.uri = "/app.js",
                          .method = HTTP_GET,
                          .handler = http_server_app_js_handler,
                          .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &app_js);

    // register quantum leap logo
    httpd_uri_t ql_logo = {.uri = "/quantum_leap.png",
                           .method = HTTP_GET,
                           .handler = http_server_ql_logo_png_handler,
                           .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &ql_logo);

    // register fonte viva logo
    httpd_uri_t fv_logo = {.uri = "/fonte_viva.png",
                           .method = HTTP_GET,
                           .handler = http_server_fv_logo_png_handler,
                           .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &fv_logo);

    // register scan networks endpoint
    httpd_uri_t scan_networks = {.uri = "/scan_networks",
                                 .method = HTTP_GET,
                                 .handler = http_server_scan_networks_handler,
                                 .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &scan_networks);

    // start provision
    httpd_uri_t provision = {.uri = "/provision",
                             .method = HTTP_GET,
                             .handler = http_server_start_provision_handler,
                             .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &provision);

    // provision status
    httpd_uri_t status = {.uri = "/status",
                          .method = HTTP_GET,
                          .handler = http_server_status_handler,
                          .user_ctx = NULL};
    httpd_register_uri_handler(http_server_handle, &status);

    return ESP_OK;
  }

  return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t http_server_start()
{
  if (http_server_handle == NULL)
  {
    return http_server_configure();
  }

  return ESP_ERR_INVALID_STATE;
}

void http_server_stop(void)
{
  if (http_server_handle)
  {
    httpd_stop(http_server_handle);
    ESP_LOGI(TAG, "http_server_stop: stopping HTTP server");
    http_server_handle = NULL;
  }
  if (task_http_server_monitor)
  {
    vTaskDelete(task_http_server_monitor);
    ESP_LOGI(TAG, "http_server_stop: stopping HTTP server monitor");
    task_http_server_monitor = NULL;
  }
}

void provision_set_status(int state) { provision_status = state; }

int provision_get_status(void) { return provision_status; }