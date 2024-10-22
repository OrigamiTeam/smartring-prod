/**
 * @file mqtt.h
 * @authors   Ayinde Olayiwola (olay@quleap.com)
 * @brief MQTT Header
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
                  __    __        __         
                 /\ \__/\ \__    /\ \        
  ___ ___      __\ \ ,_\ \ ,_\   \ \ \___    
/' __` __`\  /'__`\ \ \/\ \ \/    \ \  _ `\  
/\ \/\ \/\ \/\ \L\ \ \ \_\ \ \_  __\ \ \ \ \ 
\ \_\ \_\ \_\ \___, \ \__\\ \__\/\_\\ \_\ \_\
 \/_/\/_/\/_/\/___/\ \/__/ \/__/\/_/ \/_/\/_/
                  \ \_\                      
                   \/_/                      

 */

#ifndef __MQTT_H_
#define __MQTT_H_

/// The received topic equals the one we're testing for
#define TOPIC_OK         0

// MQTT host broker
/// MQTT host
#define MQTT_BROKER      "a2ed3xy6iprkkz-ats.iot.eu-central-1.amazonaws.com"

#define MQTT_
/// MQTT port
#define MQTT_PORT        8883
/// MQTT client prefix to append the device mac address
#define CLIENT_ID_PREFIX "smartring-"

// MQTT thread configuration
/// Thread stack size
#define MQTT_APP_TASK_STACK_SIZE 6144
/// Task priority
#define MQTT_APP_TASK_PRIORITY   2
/// Task runs on core 0
#define MQTT_APP_TASK_CORE_ID    0

// Define time interval to check version
#define TIME_VERSION_INTERVAL  2000


// Certificates

// Start of the AWS root certificate
extern const uint8_t aws_root_ca_pem_start[]     asm("_binary_aws_root_ca_pem_start");

/// End of the AWS root certificate
extern const uint8_t aws_root_ca_pem_end[]       asm("_binary_aws_root_ca_pem_end");

/// Start of the device certificate
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");

/// End of the device certificate
extern const uint8_t certificate_pem_crt_end[]   asm("_binary_certificate_pem_crt_end");

/// Start of the device private key
extern const uint8_t private_pem_key_start[]     asm("_binary_private_pem_key_start");

/// End of the device private key
extern const uint8_t private_pem_key_end[]       asm("_binary_private_pem_key_end");

/**
 * @brief
 * Type of MQTT message to create the correct message and topic.
 *
 */
typedef enum mqtt_message_type_t {

  /**
   * @brief
   * Send the sensors value when requested
   *
   * Topic : d/{device_mac}/wl
   * Info : Sensor raw value
   *
   */
  SEND_SENSORS_RESPONSE,

  /**
   * @brief
   * Send the device information, when connected
   *
   * Topic : d/{device_mac}/con
   * Info : no deposit
   *        full deposit
   *        wifi rssi
   *        Firmware version
   *        connection type
   *
   */
  SEND_DEVICE_INFORMATION,

  /**
   * @brief
   * Send device calibration
   *
   * Topic : d/{device_mac}/c
   * Info : no deposit
   *        full deposit
   *
   */
  SEND_CALIBRATION,

  /**
   * @brief
   * Send device sensors
   *
   * Topic : d/{device_mac}/wl
   * Info : Sensors raw value
   *
   */
  SEND_SENSORS,

  /**
   * @brief
   * Send gallon percenutal 
   *
   * Topic : d/{device_mac}/cons
   * Info : Percentual value
   *
   */
  SEND_PERCENTUAL,

  /**
   * @brief
   * Send device inserted login pin
   *
   * Topic : d/{device_mac}/pin
   * Info : Inserted pin
   *
   */
  SEND_LOGIN,

  /**
   * @brief
   * Send device order mode
   *
   * Topic : d/{device_mac}/o/mode
   * Info : Device order mode
   *
   */
  SEND_ORDER_MODE,

  /**
   * @brief
   * Send device stock
   *
   * Topic : d/{device_mac}/stock
   * Info : Stock
   *
   */
  SEND_STOCK,

  /**
   * @brief
   * Send device stock
   *
   * Topic : d/{device_mac}/stock
   * Info : Stock
   *
   */
  SEND_ALERT_CHNGGALLON,

  /**
   * @brief
   * Send support ticket
   *
   * Topic : d/{device_mac}/t
   * Info : Support type id number {smart_ring_support_type_t}
   *
   */
  SEND_TICKET,

  /**
   * @brief
   * Send new delivery request
   *
   * Topic : d/{device_mac}/o
   * Info : Number of bottles
   *        Number of cups
   *
   */
  SEND_NEW_DELIVERY,

  /**
   * @brief
   * Confirm new delivery
   *
   * Topic : d/{device_mac}/o/conf
   * Info : Transaction ID
   *        Confirmation character "a"
   *
   */
  SEND_CONFIRM_DELIVERY,

  /**
   * @brief
   * Get orders
   *
   * Topic : d/{device_mac}/o/list
   * Info : Empty (Since the library doesn't allow, send a space " ")
   *
   */
  SEND_GET_ORDERS,

  /**
   * @brief
   * Request device information
   *
   * Topic : d/{device_mac}/s
   * Info : 0 (Number 0 for device)
   *
   */
  SEND_REQUEST_DEVICE_INFORMATION,

  /**
   * @brief
   * Request group information
   *
   * Topic : d/{device_mac}/s
   * Info : 1 (Number 1 for group)
   *
   */
  SEND_REQUEST_GROUP_INFORMATION,

  /**
   * @brief
   * Send message to change PIN
   *
   * Topic : d/{device_mac}/c/pin
   * Info  : inserted_pin
   *
   */
  SEND_CHANGE_PIN,

  /**
   * @brief
   * Send message request latest version
   *
   * Topic : d/{device_mac}/version
   * Info  : " "
   *
   */
  SEND_REQUEST_LATESTVERSION
};

/**
 * @brief
 * Object used to controll the device MQTT interface
 *
 */
typedef struct mqtt_controller_t {
  /**
   * @brief
   * MQTT client handle
   *
   */
  AWS_IoT_Client client;

  /**
   * @brief
   * Flag signaling the task is running
   *
   */
  int running : 1;

  /**
   * @brief
   * Flag to auto-reconnect or not, used when to connect using HTTPs to update
   * firmware
   *
   */
  int auto_reconnect : 1;

  /**
   * @brief
   * Group id string retrieved from server
   *
   */
  char group_id[40];
};

/**
 * @brief
 * Send a message using the MQTT protocol to AWS server
 *
 * @param type Type of message to be sent
 * @return IoT_Error_t Result of the operation
 * @retval Success Message sent
 * @retval Other Message not sent
 */
IoT_Error_t mqtt_send_message(enum mqtt_message_type_t type);

/**
 * @brief
 * Callback triggered when the client handler receives a message from AWS
 *
 * @param client MQTT client handle
 * @param topic Topic the message was received in
 * @param topic_length The length of the {topic} string
 * @param params The parameters of the incoming data
 * @param data Information received on message
 */
static void mqtt_subscribed_handler(AWS_IoT_Client *client, char *topic,
                                    uint16_t topic_length,
                                    IoT_Publish_Message_Params *params,
                                    void *data);

/**
 * @brief
 * Callback triggered when the client handler receives a message regarding the
 * entire group from AWS
 *
 * @param client MQTT client handle
 * @param received_topic Topic the message was received in
 * @param topic_length The length of the {topic} string
 * @param params The parameters of the incoming data
 * @param data Information received on message
 */
static void mqtt_group_subscribed_handler(AWS_IoT_Client *client,
                                          char *received_topic,
                                          uint16_t topic_length,
                                          IoT_Publish_Message_Params *params,
                                          void *data);

/**
 * @brief
 * The device thread (task) containing the functioning of the AWS MQTT
 * communication
 *
 * @param param user data to be sent to the thread if needed
 */
void mqtt_aws_thread(void *param);


#endif
