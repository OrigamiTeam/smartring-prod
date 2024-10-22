/**
 * @file mqtt.c
 * @authors  Ayinde Olayiwola (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the creation and handling of
 * the MQTT communication with the server
 *
 *
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023

'##::::'##::'#######::'########:'########:::::::'######::
 ###::'###:'##.... ##:... ##..::... ##..:::::::'##... ##:
 ####'####: ##:::: ##:::: ##::::::: ##::::::::: ##:::..::
 ## ### ##: ##:::: ##:::: ##::::::: ##::::::::: ##:::::::
 ##. #: ##: ##:'## ##:::: ##::::::: ##::::::::: ##:::::::
 ##:.:: ##: ##:.. ##::::: ##::::::: ##::::'###: ##::: ##:
 ##:::: ##:. ##### ##:::: ##::::::: ##:::: ###:. ######::
..:::::..:::.....:..:::::..::::::::..:::::...:::......:::

*/

#include "libs.h"
#include "sntp.h"
#include "sntpprotocol.h"

// Tag for logging to the monitor
static const char *TAG = "MQTT";
enum mqtt_message_type_t resend_mqtt_message;

static IoT_Error_t
publish(AWS_IoT_Client *pubClient, const char *topic, char *msg, uint8_t ret, int QOS)
{

  IoT_Error_t err_mqtt;
  IoT_Publish_Message_Params params;

  uint16_t topic_len = strlen(topic);
  switch (QOS)
  {
  case 0:
    params.qos = QOS0;
    break;
  case 1:
    params.qos = QOS1;
    break;
  }

  params.payload = (void *)msg;
  params.isRetained = ret;
  params.payloadLen = strlen(msg);

  ESP_LOGI(TAG, "\n");
  ESP_LOGI(TAG, "Publishing Topic: \"%s\": \"%s\", LENGTH: %d", topic, (char *)params.payload, params.payloadLen);
  ESP_LOGI(TAG, "\n");

  // To lower the cases where the communication was failing, check if the MQTT
  // is connected before trying to send the message
  ClientState mqtt_client_state = aws_iot_mqtt_get_client_state(pubClient);
  if (mqtt_client_state > 2 && mqtt_client_state < 10)
  {

    // To prevent sending the messages in situations where the handle can't
    // process them, wait until the client is idle. Until then yild the
    // processing of the client itself to process the prending operations
    while (aws_iot_mqtt_get_client_state(pubClient) != CLIENT_STATE_CONNECTED_IDLE)
    {
      aws_iot_mqtt_yield(&smart_ring_get_controller()->connection.mqtt_controller.client, 100);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    int retries = 0;
    // Try to publish the message
    do
    {
      err_mqtt = aws_iot_mqtt_publish(pubClient, topic, topic_len, &params);
      retries++;
#ifdef NDEBUG
      ESP_LOGI(TAG, "Message published : %d", err_mqtt);
#endif
    } while (SUCCESS != err_mqtt && retries < 2);
  }
  else
  {
    err_mqtt = -1;
  }

  return err_mqtt;
}

IoT_Error_t mqtt_send_message(enum mqtt_message_type_t type)
{

  char topic[32], message[256] = "";
  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  switch (type)
  {
  case SEND_DEVICE_INFORMATION:
    sprintf(topic, "d/%s/con", smart_ring_get_mac_address());
    sprintf(message, "{\"n\":%d,\"e\":%d,\"f\":%d,\"r\":%d,\"fw\":\"%s\",\"t\":\"%c\"}",
            controller->sensor.no_deposit, controller->sensor.no_deposit,
            controller->sensor.full_deposit, wifi_get_rssi(), FIRMWARE_VERSION,
            controller->connection.type);
    break;
  case SEND_SENSORS:
    sprintf(topic, "d/%s/wl", smart_ring_get_mac_address());
    sprintf(message, "%d", controller->sensor.current_deposit);
    break;
  case SEND_PERCENTUAL:
    sprintf(topic, "d/%s/consump", smart_ring_get_mac_address());
    sprintf(message, "%d", (int)map(controller->sensor.current_deposit, controller->sensor.no_deposit, controller->sensor.full_deposit, 0, 100));
    break;
  case SEND_LOGIN:
    sprintf(topic, "d/%s/pin", smart_ring_get_mac_address());
    sprintf(message, "%s", pin_get_inserted_pin());
    break;
  case SEND_CHANGE_PIN:
    sprintf(topic, "d/%s/c/pin", smart_ring_get_mac_address());
    sprintf(message, "%s", pin_get_inserted_pin());
    break;
  case SEND_NEW_DELIVERY:
    sprintf(topic, "d/%s/o", smart_ring_get_mac_address());
    sprintf(message, "{\"qb\":%d,\"qc\":%d}", controller->ui_controller->order.bottles,
            controller->ui_controller->order.cups);
    break;
  case SEND_CONFIRM_DELIVERY:
    sprintf(topic, "d/%s/o/conf", smart_ring_get_mac_address());
    sprintf(message, "{\"id\":\"%s\",\"qb\":%d,\"qc\":%d,\"c\":\"%c\"}",
            controller->ui_controller->order.transaction_id,
            controller->ui_controller->order.bottles,
            controller->ui_controller->order.cups,
            controller->ui_controller->order.confirmation);
    break;
  case SEND_GET_ORDERS:
    sprintf(topic, "d/%s/o/list", smart_ring_get_mac_address());
    sprintf(message, " ");
    break;
  case SEND_ORDER_MODE:
    sprintf(topic, "d/%s/o/mode", smart_ring_get_mac_address());
    sprintf(message, "\"%c\"", controller->ui_controller->updated_order_mode);
    break;
  case SEND_TICKET:
    sprintf(topic, "d/%s/t", smart_ring_get_mac_address());
    sprintf(message, "%d", controller->ui_controller->support_type);
    break;
  case SEND_STOCK:
    sprintf(topic, "d/%s/stock", smart_ring_get_mac_address());
    sprintf(message, controller->ui_controller->flags.flag.update_stock_manual ? "{\"au\":%d}" : "{\"mu\":%d}", controller->ui_controller->updated_stock); // au means automatic update when bottle is replaced
    controller->ui_controller->flags.flag.update_stock_manual = false;                                                                                     // mu means manual update is when stock is updated maually
    break;
  case SEND_ALERT_CHNGGALLON:
    sprintf(topic, "d/%s/change", smart_ring_get_mac_address());
    sprintf(message, " "); // mu means manual update is when stock is updated maually
    break;
  case SEND_REQUEST_DEVICE_INFORMATION:
    sprintf(topic, "d/%s/s", smart_ring_get_mac_address());
    sprintf(message, "%d", 0);
    break;
  case SEND_REQUEST_GROUP_INFORMATION:
    sprintf(topic, "d/%s/s", smart_ring_get_mac_address());
    sprintf(message, "%d", 1);
    break;
  case SEND_SENSORS_RESPONSE:
    sprintf(topic, "d/%s/wl/res", smart_ring_get_mac_address());
    sprintf(message, "%d", controller->sensor.current_deposit);
    break;
  case SEND_CALIBRATION:
    sprintf(topic, "d/%s/c", smart_ring_get_mac_address());
    sprintf(message, "{\n\"n\":%d,\"e\":%d,\"f\":%d\n}", controller->sensor.no_deposit,
            controller->sensor.no_deposit, controller->sensor.full_deposit);
    break;
  case SEND_REQUEST_LATESTVERSION:
    sprintf(topic, "d/%s/version", smart_ring_get_mac_address());
    sprintf(message, "%s", FIRMWARE_VERSION);
    break;
  default:
    ESP_LOGE(TAG, "Invalid MQTT message type");
    break;
  }
  resend_mqtt_message = type;

  IoT_Error_t pub_error = publish(&controller->connection.mqtt_controller.client, topic, message, 0, 1);

  if (SUCCESS != pub_error)
  {
    ESP_LOGE(TAG, "Error sending MQTT message : %d", pub_error);
  }

  return pub_error;
}

static void mqtt_subscribe_to_topics(void)
{
  struct smart_ring_controller_t *controller = smart_ring_get_controller();
  IoT_Error_t err_mqtt = FAILURE;

// Subscribe to device specific topics
#ifndef NDEBUG
  ESP_LOGI(TAG, "Subscribing to topics");
#endif

  char topic[40];

  // Subscribe to device entire tree topics
  sprintf(topic, "sd/%s/#", controller->mac_address);
  err_mqtt = aws_iot_mqtt_subscribe(&controller->connection.mqtt_controller.client, topic, strlen(topic),
                                    QOS0, mqtt_subscribed_handler, NULL);
  if (SUCCESS != err_mqtt)
  {
    ESP_LOGE(TAG, "Error subscribing : %d ", err_mqtt);
    return;
  }

#ifndef NDEBUG
  ESP_LOGI(TAG, "Subscribed to device specific topics\n\nsd/%s/+\nsd/%s/+/+\n",
           controller->mac_address, controller->mac_address);
#endif

  // Send the device connected information
  err_mqtt = mqtt_send_message(SEND_DEVICE_INFORMATION);
  if (SUCCESS != err_mqtt)
  {
    // Max time the yield function will wait for read messages
    if (err_mqtt == MQTT_REQUEST_TIMEOUT_ERROR)
    {
      ESP_LOGE(TAG, "QOS1 publish ack not received.");
    }
    else
    {
      ESP_LOGE(TAG, "Error sending MQTT message : %d", err_mqtt);
    }

    return;
  }

  // Get the group id information to subscribe to group topics
  err_mqtt = mqtt_send_message(SEND_REQUEST_DEVICE_INFORMATION);
  if (SUCCESS != err_mqtt)
  {
    // Max time the yield function will wait for read messages
    if (err_mqtt == MQTT_REQUEST_TIMEOUT_ERROR)
    {
      ESP_LOGE(TAG, "QOS1 publish ack not received.");
    }
    else
    {
      ESP_LOGE(TAG, "Error sending MQTT message : %d", err_mqtt);
    }

    return;
  }

  ESP_LOGI(TAG, "Requested device group information");

  int try = 0;
  // Get the group id information to update device configuration
  while (try < 3)
  {
    err_mqtt = mqtt_send_message(SEND_REQUEST_GROUP_INFORMATION);
    try++;

    vTaskDelay(portTICK_PERIOD_MS);
  }
  if (SUCCESS != err_mqtt)
  {
    // Max time the yield function will wait for read messages
    if (err_mqtt == MQTT_REQUEST_TIMEOUT_ERROR)
    {
      ESP_LOGE(TAG, "QOS1 publish ack not received.");
    }
    else
    {
      ESP_LOGE(TAG, "Error sending MQTT message : %d", err_mqtt);
    }

    return;
  }

  ESP_LOGI(TAG, "Requested device group configuration");

  return;
}

static void mqtt_subscribe_to_group_topics()
{

  // Subscribe to group specific topics
  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  IoT_Error_t err_mqtt = FAILURE;

// Subscribe to group specific topics
#ifndef NDEBUG
  ESP_LOGI(TAG, "Subscribing to group topics");
#endif

  char topic[60];

  // Subscribe to group entire tree topics
  sprintf(topic, "g/%s/#", controller->connection.mqtt_controller.group_id);
  err_mqtt = aws_iot_mqtt_subscribe(&controller->connection.mqtt_controller.client, topic, strlen(topic),
                                    QOS0, mqtt_group_subscribed_handler, NULL);
  if (SUCCESS != err_mqtt)
  {
    ESP_LOGE(TAG, "Error subscribing : %d ", err_mqtt);
    return;
  }

#ifndef NDEBUG
  ESP_LOGI(TAG, "Subscribed to group specific topics\n\ng/%s/+\ng/%s/+/+\n",
           controller->connection.mqtt_controller.group_id,
           controller->connection.mqtt_controller.group_id);
#endif

  return;
}

// Outdated: Not used
static void mqtt_group_subscribed_handler(AWS_IoT_Client *client, char *received_topic, uint16_t topic_length,
                                          IoT_Publish_Message_Params *params, void *data)
{

  struct smart_ring_controller_t *controller = smart_ring_get_controller();
  IoT_Error_t err_mqtt = FAILURE;

  // Get the information from data received
  char topic[topic_length];
  strncpy(topic, received_topic, topic_length);
  topic[topic_length] = '\0';

  char payload[params->payloadLen];
  strncpy(payload, params->payload, params->payloadLen);
  payload[params->payloadLen] = '\0';

#ifndef NDEBUG
  ESP_LOGI(TAG, "\n\n============ MESSAGE RECEIVED =============\n\n\nTOPIC : %s \nPAYLOAD : %s "
                "\n\n================================================\n\n",
           topic, payload);
#endif

  char topic_prefix[45];
  sprintf(topic_prefix, "g/%s", controller->connection.mqtt_controller.group_id);
}

// Function to receive messages from MQTT
static void mqtt_subscribed_handler(AWS_IoT_Client *client, char *received_topic, uint16_t topic_length,
                                    IoT_Publish_Message_Params *params, void *data)
{

  struct smart_ring_controller_t *controller = smart_ring_get_controller();
  IoT_Error_t err_mqtt = FAILURE;

  // Get the information from data received
  char topic[topic_length];
  strncpy(topic, received_topic, topic_length);

  printf("Received Topic: %s", received_topic);
  printf("Received Length: %d", topic_length);

  topic[topic_length] = '\0';

  char payload[params->payloadLen + 1];
  strncpy(payload, params->payload, params->payloadLen);
  payload[params->payloadLen] = '\0';

  ESP_LOGI(TAG, "\n\n============ MESSAGE RECEIVED =============\n\n\nTOPIC : %s \nPAYLOAD : %s "
                "\n\n================================================\n\n",
           topic, payload);

  char topic_prefix[25];
  sprintf(topic_prefix, "sd/%s", smart_ring_get_mac_address());
#ifndef NDEBUG
  ESP_LOGI(TAG, "Topic prefix : %s", topic_prefix);
#endif

  char topic_to_compare[40];

  // Device information
  sprintf(topic_to_compare, "%s/s", topic_prefix);
  if (strcmp(topic_to_compare, topic) == TOPIC_OK)
  {
#ifndef NDEBUG
    ESP_LOGI(TAG, "Received device information");
#endif

    // Parse to JSON
    const cJSON *item = NULL;
    cJSON *payload_json = cJSON_Parse(payload);

    if (payload_json == NULL)
    {
      const char *error_ptr = cJSON_GetErrorPtr();
      if (error_ptr != NULL)
      {
        ESP_LOGE(TAG, "Error parsing JSON object : %s", error_ptr);
      }
      return;
    }

    // Get information from JSON object
    // Get device name
    item = cJSON_GetObjectItemCaseSensitive(payload_json, "n");
    if (cJSON_IsString(item) && (item->valuestring != NULL))
    {
      smart_ring_set_device_name(item->valuestring);
    }

    // Get group id
    item = cJSON_GetObjectItemCaseSensitive(payload_json, "g");
    if (cJSON_IsString(item) && (item->valuestring != NULL))
    {
      strcpy(controller->connection.mqtt_controller.group_id, item->valuestring);
    }

#ifndef NDEBUG
    ESP_LOGI(TAG, "Saved device information\n\nName : %s\nGroup : %s", smart_ring_get_device_name(),
             controller->connection.mqtt_controller.group_id);
#endif

    // Subscribe to group topics
    mqtt_subscribe_to_group_topics();

    // Register the current water level
    controller->ui_controller->flags.flag.register_water_level = true;
    cJSON_Delete(payload_json);

    return;
  }
  
  // New version information
  sprintf(topic_to_compare, "%s/version", topic_prefix);
  if (strcmp(topic_to_compare, topic) == TOPIC_OK)
  {
    ESP_LOGI(TAG, "Received device information");

    // Parse to JSON
    const cJSON *item = NULL;
    cJSON *payload_json = cJSON_Parse(payload);

    if (payload_json == NULL)
    {
      const char *error_ptr = cJSON_GetErrorPtr();
      if (error_ptr != NULL)
      {
        ESP_LOGE(TAG, "Error parsing JSON object : %s", error_ptr);
      }
      return;
    }

    switch (payload[1]) {
      case 't':
        smart_ring_ui_get_controller()->newVersion = true;
        ESP_LOGI(TAG,"New Version available!");
        break;
    }
  }

  // Confirmation validation
  sprintf(topic_to_compare, "%s/o/conf", topic_prefix);
  if (strcmp(topic_to_compare, topic) == TOPIC_OK)
  {
#ifndef NDEBUG
    ESP_LOGI(TAG, "Received order delivery confirmation");
#endif
    if (payload[1] == 's')
    {
      smart_ring_ui_update_state(STATE_27);
#ifndef NDEBUG
      ESP_LOGI(TAG, "Confirmation Validation data received is successful\n\n");
#endif
    }
    else
    {
      smart_ring_ui_update_state(STATE_26);
#ifndef NDEBUG
      ESP_LOGI(TAG, "Confirmation Validation data received is failed - payload: %c\n\n", payload[1]);
#endif
    }
    controller->ui_controller->flags.flag.order_confirmed_response = true;
  }

  // PIN validation
  sprintf(topic_to_compare, "%s/pin", topic_prefix);
  if (strcmp(topic, topic_to_compare) == TOPIC_OK)
  {

    if (controller->ui_controller->state != STATE_8)
    {
      return;
    }

    esp_timer_stop(controller->ui_controller->timer);

#ifndef NDEBUG
    ESP_LOGI(TAG, "Received pin validation information");
#endif

    // Parse to JSON
    const cJSON *item = NULL;

    cJSON *payload_json = cJSON_Parse(payload);
    if (payload_json == NULL)
    {
      const char *error_ptr = cJSON_GetErrorPtr();
      if (error_ptr != NULL)
      {
        ESP_LOGE(TAG, "Error parsing JSON object : %s", error_ptr);
      }
      return;
    }

    // Get information from JSON object
    // Get user_name
    item = cJSON_GetObjectItemCaseSensitive(payload_json, "n");
    if (cJSON_IsString(item) && (item->valuestring != NULL))
    {
      smart_ring_set_user_name(item->valuestring);
    }
    else
    {
      // Throw error
      smart_ring_ui_update_state(STATE_9);
      return;
    }

    // Get user_role
    item = cJSON_GetObjectItemCaseSensitive(payload_json, "r");
    if (cJSON_IsString(item) && (item->valuestring != NULL))
    {
      smart_ring_set_user_role(item->valuestring[1]);
    }

#ifndef NDEBUG
    ESP_LOGI(TAG, "PIN Validation data received\n\nName : %s\nRole : %c",
             smart_ring_get_user_name(), smart_ring_get_user_role());
#endif

    smart_ring_ui_update_state(STATE_10); // CHECK #2
    cJSON_Delete(payload_json);

    return;
  }

  // PIN updation
  sprintf(topic_to_compare, "%s/c/pin", topic_prefix);
  if (strcmp(topic, topic_to_compare) == TOPIC_OK)
  {

    if (controller->ui_controller->state != STATE_44)
    {
      return;
    }

    esp_timer_stop(controller->ui_controller->timer);

#ifndef NDEBUG
    ESP_LOGI(TAG, "Received pin updation information");
#endif

    char change_pin_result = payload[1];

#ifndef NDEBUG
    ESP_LOGI(TAG, "PIN Updation data received - %c\n\n", change_pin_result);
#endif

    if (change_pin_result == 's')
      smart_ring_ui_update_state(STATE_10);
    else
      smart_ring_ui_update_state(STATE_45);

    return;
  }

  // Device configuration
  sprintf(topic_to_compare, "%s/info", topic_prefix);
  if (strcmp(topic, topic_to_compare) == TOPIC_OK)
  {
#ifndef NDEBUG
    ESP_LOGI(TAG, "Device configuration received");
#endif
    // Parse to JSON
    const cJSON *item = NULL;

    cJSON *payload_json = cJSON_Parse(payload);
    if (payload_json == NULL)
    {
      const char *error_ptr = cJSON_GetErrorPtr();
      if (error_ptr != NULL)
      {
        ESP_LOGE(TAG, "Error parsing JSON object : %s", error_ptr);
      }
      return;
    }

    // Get information from JSON object
    // Get order mode
    item = cJSON_GetObjectItemCaseSensitive(payload_json, "o");
    if (cJSON_IsString(item) && (item->valuestring != NULL))
    {
      smart_ring_set_order_mode(item->valuestring[0]);
    }

    // Get stock
    item = cJSON_GetObjectItemCaseSensitive(payload_json, "s");
    if (cJSON_IsNumber(item))
    {
      smart_ring_set_stock(item->valueint);
    }

    // Change state if on boot screen
    if (smart_ring_ui_get_controller()->state == STATE_4)
    {
      smart_ring_ui_update_state(STATE_5);
    }
    cJSON_Delete(payload_json);
  }

  // Order list
  sprintf(topic_to_compare, "%s/o/list", topic_prefix);
  if (strcmp(topic, topic_to_compare) == TOPIC_OK)
  {

#ifndef NDEBUG
    ESP_LOGI(TAG, "Order list received");
#endif
    // Parse to JSON
    const cJSON *delivery = NULL;
    const cJSON *item = NULL;

    cJSON *payload_json = cJSON_Parse(payload);

    if (payload_json == NULL)
    {
      const char *error_ptr = cJSON_GetErrorPtr();
      if (error_ptr != NULL)
      {
        ESP_LOGE(TAG, "Error parsing JSON object : %s", error_ptr);
      }
      return;
    }

    // Get delivery from array
    if (!cJSON_IsArray(payload_json))
    {
      ESP_LOGE(TAG, "Information is not a array");
      return;
    }

    int number_of_items = cJSON_GetArraySize(payload_json);
    ESP_LOGI(TAG, " Received %d deliveries", number_of_items);

    free(controller->ui_controller->deliveries);
    controller->ui_controller->deliveries = NULL;

    if (number_of_items == 0)
    {
      controller->ui_controller->number_of_deliveries = 0;
      if (controller->ui_controller->timer_type == REQUESTING_ORDERS)
      {
        esp_timer_stop(controller->ui_controller->timer);
        smart_ring_ui_update_state(STATE_18);
      }

      // Check by menu, becuase state can state before object is created
      if (controller->ui_controller->menu == MENU_ID_MAIN)
      {
        smart_ring_ui_main_clear_next_eta();
      }

      if (controller->ui_controller->state == STATE_35)
      {
        esp_timer_stop(controller->ui_controller->timer);
        smart_ring_ui_update_state(STATE_47);
      }

      return;
    }

    controller->ui_controller->number_of_deliveries = number_of_items;

    // Reallocate the deliveries list
    controller->ui_controller->deliveries = (struct smart_ring_ui_delivery_t *)malloc(
        (number_of_items + 1) * sizeof(struct smart_ring_ui_delivery_t));

    int pointer = 0;
    cJSON_ArrayForEach(delivery, payload_json)
    {
      if (pointer >= 5)
      {
        break;
      }

      // Get ETA
      item = cJSON_GetObjectItemCaseSensitive(delivery, "e");
      if (cJSON_IsString(item) && (item->valuestring != NULL))
      {
        strcpy(controller->ui_controller->deliveries[pointer].date, item->valuestring);
      }

      // Get ordered date
      item = cJSON_GetObjectItemCaseSensitive(delivery, "d");
      if (cJSON_IsString(item) && (item->valuestring != NULL))
      {
        strcpy(controller->ui_controller->deliveries[pointer].ordered_date, item->valuestring);
      }

      // Get bottles
      item = cJSON_GetObjectItemCaseSensitive(delivery, "qb");
      if (cJSON_IsNumber(item))
      {
        controller->ui_controller->deliveries[pointer].bottles = item->valueint;
      }

      // Get cups
      item = cJSON_GetObjectItemCaseSensitive(delivery, "qc");
      if (cJSON_IsNumber(item))
      {
        controller->ui_controller->deliveries[pointer].cups = item->valueint;
      }

      // Get state
      item = cJSON_GetObjectItemCaseSensitive(delivery, "s");
      if (cJSON_IsString(item) && (item->valuestring != NULL))
      {
        strcpy(controller->ui_controller->deliveries[pointer].status, item->valuestring);
      }

      controller->ui_controller->deliveries[pointer].ordered_date[10] = '\0';
      controller->ui_controller->deliveries[pointer].date[10] = '\0';

#ifndef NDEBUG
      ESP_LOGI(TAG,
               "\n===========================\nOrder #%d\n\nCreated at : "
               "%s\nETA : %s\n\nBottles : %d\nCups : %d\nStatus : "
               "%s\n\n===============================\n\n",
               pointer + 1,
               controller->ui_controller->deliveries[pointer].ordered_date,
               controller->ui_controller->deliveries[pointer].date,
               controller->ui_controller->deliveries[pointer].bottles,
               controller->ui_controller->deliveries[pointer].cups,
               controller->ui_controller->deliveries[pointer].status);
#endif

      pointer++;
    }

    controller->ui_controller->number_of_deliveries = pointer;

    // Set the next delivery to be delivered
    if (controller->ui_controller->state == STATE_5)
    {
      if (pointer > 0)
      {
        smart_ring_ui_main_update_next_eta();
      }
      else
      {
        smart_ring_ui_main_clear_next_eta();
      }
    }

    esp_timer_stop(controller->ui_controller->timer);
    if (controller->ui_controller->state == STATE_35)
    {
      smart_ring_ui_update_state(STATE_47);
    }
    else if (controller->ui_controller->state == STATE_11)
    {
      smart_ring_ui_update_state(STATE_18);
    }

    cJSON_Delete(payload_json);
  }

  // New order information
  sprintf(topic_to_compare, "%s/o", topic_prefix);
  if (strcmp(topic, topic_to_compare) == TOPIC_OK)
  {
    if (controller->ui_controller->state == STATE_23)
    {
#ifndef NDEBUG
      ESP_LOGI(TAG, "Received order information");
#endif

      // Parse to JSON
      const cJSON *item = NULL;

      cJSON *payload_json = cJSON_Parse(payload);
      if (payload_json == NULL)
      {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
          ESP_LOGE(TAG, "Error parsing JSON object : %s", error_ptr);
        }
        return;
      }

      // Get information from JSON object
      // Get order id
      item = cJSON_GetObjectItemCaseSensitive(payload_json, "id");
      if (cJSON_IsString(item) && (item->valuestring != NULL))
      {
        strcpy(controller->ui_controller->order.transaction_id,
               item->valuestring);
      }

      // Get price
      item = cJSON_GetObjectItemCaseSensitive(payload_json, "tp");
      if (cJSON_IsNumber(item))
      {
        controller->ui_controller->order.price = item->valuedouble / 100.0;
      }
      if (cJSON_IsNumber(controller->ui_controller->order.price == 0.00))
      {
        mqtt_send_message(resend_mqtt_message);
#ifndef NDEBUG
        ESP_LOGE(TAG, "API ERROR [resending message]");
#endif
        return;
      }

#ifndef NDEBUG
      ESP_LOGI(TAG,
               "Order information\n\tID : %s\n\tBottles : %d\n\t Cups : "
               "%d\n\tPrice : %.2f",
               controller->ui_controller->order.transaction_id,
               controller->ui_controller->order.bottles,
               controller->ui_controller->order.cups,
               controller->ui_controller->order.price);
#endif

      smart_ring_ui_order_update_review_data(controller->ui_controller->order.price);
      cJSON_Delete(payload_json);
    }
  }
}

// TODO : Create mqtt_disconnect_handler
static void mqtt_disconnect_handler(AWS_IoT_Client *client, void *data)
{
  IoT_Error_t err_mqtt = FAILURE;

  if (NULL == client)
  {
    return;
  }

  ESP_LOGW(TAG, "MQTT Disconnect");

#ifndef NDEBUG
  ESP_LOGI(TAG, "Auto reconnect : %s", smart_ring_get_controller()->connection.mqtt_controller.auto_reconnect ? "ON" : "OFF");
#endif
  if (smart_ring_get_controller()->connection.mqtt_controller.auto_reconnect)
  {
    if (aws_iot_is_autoreconnect_enabled(client))
    {
      ESP_LOGI(TAG, "Auto Reconnect is enabled, Reconnecting attempt will start now");
    }
    else
    {
      ESP_LOGW(TAG, "Auto Reconnect not enabled. Starting manual reconnect...");

      err_mqtt = aws_iot_mqtt_attempt_reconnect(client);
      if (NETWORK_RECONNECTED == err_mqtt)
      {
        ESP_LOGW(TAG, "Manual Reconnect Successful");
      }
      else
      {
        ESP_LOGW(TAG, "Manual Reconnect Failed - %d", err_mqtt);
      }
    }
  }
}

void mqtt_aws_thread(void *param)
{

  while (!smart_ring_get_controller()->connection.is_connected)
  {
#ifndef NDEBUG
    ESP_LOGI(TAG, "WiFi not connected");
#endif
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  char *certificatePem = (char *)calloc(2000, sizeof(char));
  char *privateKey = (char *)calloc(2000, sizeof(char));

  if (nvs_load_temporary_mqtt_certificate_pem(certificatePem) != ESP_OK)
  {
    boot_update_warning_label("  A configurar o MQTT");
    smart_ring_http_client_get_certificate();
    boot_update_warning_label("A reiniciar...");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    esp_restart();
  }
  spiffs.loadMQTTCertificatePrivateKey(privateKey);

#ifndef NDEBUG
  printf("----  NVS Certificate ------ \n%s", certificatePem);
  printf("\n%s", privateKey);
#endif

  // Signal the thread is running
  smart_ring_get_controller()->connection.mqtt_controller.running = true;

  IoT_Error_t err_mqtt = FAILURE;
  IoT_Client_Init_Params mqtt_init_config = iotClientInitParamsDefault;
  IoT_Client_Connect_Params mqtt_connect_config = iotClientConnectParamsDefault;

  ESP_LOGI(TAG, "AWS IoT SDK Version %d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

  mqtt_init_config.enableAutoReconnect = true;
  mqtt_init_config.disconnectHandler = mqtt_disconnect_handler;
  mqtt_init_config.pHostURL = MQTT_BROKER;
  mqtt_init_config.port = MQTT_PORT;
  mqtt_init_config.pRootCALocation = (const char *)aws_root_ca_pem_start;
  mqtt_init_config.pDeviceCertLocation = (const char *)certificatePem;
  mqtt_init_config.pDevicePrivateKeyLocation = (const char *)privateKey;
  mqtt_init_config.mqttCommandTimeout_ms = 20000;
  mqtt_init_config.tlsHandshakeTimeout_ms = 5000;
  mqtt_init_config.isSSLHostnameVerify = true;
  mqtt_init_config.disconnectHandlerData = NULL;

#ifndef NDEBUG
  ESP_LOGI(TAG, "MQTT configured, initializing");
#endif

  struct mqtt_controller_t *mqtt_controller = &smart_ring_get_controller()->connection.mqtt_controller;

  // Try to connect to the MQTT broker
  err_mqtt = aws_iot_mqtt_init(&mqtt_controller->client, &mqtt_init_config);

  if (err_mqtt != SUCCESS)
  {
    ESP_LOGE(TAG, "Error initializing MQTT : %d", err_mqtt);
    mqtt_controller->running = false;
    vTaskDelete(NULL);
  }

  char topic_last_will[27];
  sprintf(topic_last_will, "d/%s/discon", smart_ring_get_mac_address());

  char *message_last_will = "";

  IoT_MQTT_Will_Options last_will_options = {
      .isRetained = false,
      .pMessage = message_last_will,
      .msgLen = strlen(message_last_will),
      .pTopicName = topic_last_will,
      .topicNameLen = strlen(topic_last_will),
      .qos = 1,
  };

#ifndef NDEBUG
  ESP_LOGI(TAG, "Last will configured");
#endif

  char client_id[100];
  sprintf(client_id, "%s%s", CLIENT_ID_PREFIX, smart_ring_get_mac_address());

  mqtt_connect_config.keepAliveIntervalInSec = 60;
  mqtt_connect_config.isCleanSession = true;
  mqtt_connect_config.MQTTVersion = MQTT_3_1_1;
  mqtt_connect_config.pClientID = client_id;
  mqtt_connect_config.clientIDLen = (uint16_t)strlen(client_id);
  mqtt_connect_config.isWillMsgPresent = true;
  mqtt_connect_config.will = last_will_options;

  boot_update_warning_label("A conectar ao servidor");

#ifndef NDEBUG
  ESP_LOGI(TAG, "Connection parameters configured");
  ESP_LOGI(TAG, "Connecting to AWS Server\n\nServer :\n\tHOST : %s\n\tPORT : %d\nClient ID : %s\n",
           MQTT_BROKER, MQTT_PORT, client_id);
#endif

  uint32_t free_heap_size = 0,
           min_free_heap_size = 0,
           heap_caps_size = 0,
           heap_caps_free_block = 0,
           heap_caps_minimum = 0;

  free_heap_size = esp_get_free_heap_size();
  min_free_heap_size = esp_get_minimum_free_heap_size();
  heap_caps_size = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
  heap_caps_free_block = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT);

  printf("\nheap caps = %d \t  heap free block = %d \t  free heap size = %d \t  min_free_heap_size = %d \n",
         heap_caps_size, heap_caps_free_block, free_heap_size, min_free_heap_size);

  ESP_LOGI(TAG, "Stack remaining for task '%s' is %d bytes", pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));
  int retries = 0;

#ifndef NDEBUG
  ESP_LOGI(TAG, "Timers status\n\tTimer: %s\n\tStandby: %s", esp_timer_is_active(smart_ring_ui_get_controller()->timer) ? "Active" : "NOK",
           esp_timer_is_active(smart_ring_ui_get_controller()->standby_timer) ? "Active" : "NOK");
#endif

  vTaskDelay(500 / portTICK_PERIOD_MS);

  do
  {
    ESP_LOGI(TAG, "Stack remaining for task '%s' is %d bytes", pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));
    err_mqtt = aws_iot_mqtt_connect(&mqtt_controller->client, &mqtt_connect_config);
    if (SUCCESS != err_mqtt)
    {
      ESP_LOGE(TAG, "Error(%d) connecting to %s:%d as %s", err_mqtt, mqtt_init_config.pHostURL, mqtt_init_config.port, client_id);
      vTaskDelay(1000 / portTICK_RATE_MS);
      retries++;
    }
  } while (err_mqtt != SUCCESS && retries < 3);

  // Enable reconnect
  err_mqtt = aws_iot_mqtt_autoreconnect_set_status(&mqtt_controller->client, true);
  if (SUCCESS != err_mqtt)
  {
    ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", err_mqtt);
    vTaskDelete(NULL);
  }

  if (retries < 3)
  {
    boot_update_warning_label("A obter configurações");
    mqtt_subscribe_to_topics(); // CHECK #1

    // Get controller for flags
    struct smart_ring_controller_t *controller = smart_ring_get_controller();

    initialize_sntp();
    wait_for_sntp_sync();
    set_timezone();
  }
  else
  {
    boot_update_warning_label("Erro de conexão");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    smart_ring_ui_update_state(STATE_5);
  }

  free(certificatePem);
  free(privateKey);

  ClientState client_state;
  bool client_connected_state;
  static TickType_t lastTimePercCheck = 0;
  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  for (;;)
  {

    // Max time the yield function will wait for read messages
    err_mqtt = aws_iot_mqtt_yield(&mqtt_controller->client, 100);

    /*
    // Get client state information
    client_state = aws_iot_mqtt_get_client_state(&mqtt_controller->client);
    */  

    client_connected_state = aws_iot_mqtt_is_client_connected(&mqtt_controller->client);
    ESP_LOGW(TAG, "Client Connection: %d", client_connected_state);

    if ((client_connected_state == 0) && !(smart_ring_get_controller()->ui_controller->state == STATE_34)){
      smart_ring_ui_update_state(STATE_48);
    } 
    

    if (xTaskGetTickCount() - lastTimePercCheck >= TIME_VERSION_INTERVAL)
    {
      ESP_LOGI(TAG, "Checking time for VersionCheck");

      // Verify if it is time to send version check message
      if (versionCheck(controller))
      {
        ESP_LOGI(TAG, "Sending message with device version...");
        mqtt_send_message(SEND_REQUEST_LATESTVERSION);
        controller->flags.flag.sent_version_check_message = true;
      }

      lastTimePercCheck = xTaskGetTickCount(); // Update the last checked time
    }

    if (NETWORK_ATTEMPTING_RECONNECT == err_mqtt)
    {
      // If the client is attempting to reconnect we will skip the rest of
      // the loop.
      continue;
    }

    vTaskDelay(1000 / portTICK_RATE_MS);
  }

  ESP_LOGE(TAG, "Error ocurred in the mqtt loop");
  mqtt_controller->running = false;
  vTaskDelete(NULL);
} 