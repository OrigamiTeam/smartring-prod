/**
 * @file vars.h
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Variables header
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
                                 __         

██    ██  █████  ██████  ███████    ██   ██ 
██    ██ ██   ██ ██   ██ ██         ██   ██ 
██    ██ ███████ ██████  ███████    ███████ 
 ██  ██  ██   ██ ██   ██      ██    ██   ██ 
  ████   ██   ██ ██   ██ ███████ ██ ██   ██ 
                                                                                      
                                                                                   
 */

#ifndef __VARS_H_
#define __VARS_H_

// Firmware version to be used on the UI, version check and Back-Office
#define FIRMWARE_VERSION          "3.2.3"

// Refresh rate for the display interface
#define LV_TICK_PERIOD_MS         1

// Wifi related settings
// Provision AP network name
#define WIFI_AP_SSID              "Smartring"
/// Provision AP network password
#define WIFI_AP_PASSWORD          "fonteviva"
/// Provision AP network channel
#define WIFI_AP_CHANNEL           1
/// Provision AP network visibility
#define WIFI_AP_SSID_HIDDEN       0
/// Provision AP network max connections
#define WIFI_AP_MAX_CONNECTIONS   5
/// Provision AP network beacon interval
#define WIFI_AP_BEACON_INTERVAL   100
/// Provision AP network device ip
#define WIFI_AP_IP                "192.168.0.1"
/// Provision AP network device ip
#define WIFI_AP_GATEWAY           "192.168.0.1"
/// Provision AP network mask
#define WIFI_AP_NETMASK           "255.255.255.0"
/// Provision AP network bandwidtch
#define WIFI_AP_BANDWIDTH         WIFI_BW_HT20
/// Provision AP network power save mode
#define WIFI_STA_POWER_SAVE       WIFI_PS_NONE

/**
 * @brief
 * Controller used to store the wifi interface data
 */
typedef struct smart_ring_wifi_controller_t {
  /**
   * @brief
   * Wifi AP interface used to clear it after provisioning
   *
   */
  esp_netif_t *esp_netif_ap;

  /**
   * @brief
   * Network SSID
   *
   */
  char ssid[32];

  /**
   * @brief
   * Network password
   *
   */
  char password[64];

  /**
   * @brief
   * Interval, in seconds, in which the device will reconnect to the saved
   * network
   *
   */
  int retry_interval;
};

/**
 * @brief  Main structures that groups the entire connection information of the
 * device
 */
typedef struct smart_ring_connection_controller_t {
  /**
   * @brief
   * Wifi interface controller object
   *
   */
  struct smart_ring_wifi_controller_t wifi_controller;

  /**
   * @brief
   * MQTT interface controller object
   *
   */
  struct mqtt_controller_t mqtt_controller;

  /**
   * @brief Bitfield to detect if a connection is established
   *
   */
  int is_connected : 1;

  /**
   * @brief
   * Bitfield to detect if the device is provisioned
   *
   */
  int is_provisioned : 1;

  /**
   * @brief
   * Type of connection - WiFi (w), GSM (g)
   *
   */
  char type;
};

/**
 * @brief
 * Device system main flags, used to signal state of the device from other
 * controllers, such as wifi or sensors
 *
 */
typedef struct smart_ring_flags_t {
  /**
   * @brief
   * Device has ended the initial configuration
   *
   */
  int end_loading_config : 1;

  /**
   * @brief
   * Update the communication type the device is using
   *
   */
  int update_communication : 1;

  /**
   * @brief
   * The firmware has finished updating
   *
   */
  int update_complete : 1;

  /**
   * @brief
   * There was an error updating the device firmware
   *
   */
  int update_failed : 1;

  /**
   * @brief
   * Device is calibrating
   *
   */
  int calibrating : 1;

  /**
   * @brief
   * Signal the device is connected to a network and can now start the MQTT
   * interface
   *
   */
  int start_mqtt : 1;

  /**
   * @brief
   * Flag the message of percentage to be sent
   *
   */
  bool sent_perc_message : true; 

  /**
   * @brief
   * Flag the message of version check
   *
   */
  bool sent_version_check_message : true; 

  /// Every flag below this one can be used before increasing the variable size
  int flag_8 : 1;
  int flag_9 : 1;
  int flag_10 : 1;
  int flag_11 : 1;
  int flag_12 : 1;
  int flag_13 : 1;
  int flag_14 : 1;
  int flag_15 : 1;
  int flag_16 : 1;
  int flag_17 : 1;
  int flag_18 : 1;
  int flag_19 : 1;
  int flag_20 : 1;
  int flag_21 : 1;
  int flag_22 : 1;
  int flag_23 : 1;
  int flag_24 : 1;
  int flag_25 : 1;
  int flag_26 : 1;
  int flag_27 : 1;
  int flag_28 : 1;
  int flag_29 : 1;
  int flag_30 : 1;
  int flag_31 : 1;
  int flag_32 : 1;
};

/**
 * @brief
 * Order information object structure
 *
 */
typedef struct smart_ring_order_t {
  /**
   * @brief
   * Transaction ID
   *
   */
  char *id;

  /**
   * @brief
   * Number of bottles
   *
   */
  uint8_t bottles;

  /**
   * @brief
   * Number of cups
   *
   */
  uint16_t cups;

  /**
   * @brief
   * Price of the order
   *
   */
  char *price;

  /**
   * @brief
   * Character meaning to confirm the order
   *
   */
  char confirm_order;
};

/**
 * @brief
 * Sensor controller object
 *
 */
typedef struct smart_ring_sensor_t {
  /**
   * @brief
   * Calibrating process structure
   *
   */
  struct smart_ring_sensor_calirabting_t {
    /**
     * @brief
     * Used to detect stability, only after 10 seconds of stability we consider
     * the step valid
     *
     */
    long timestamp;

    /**
     * @brief
     * Sum of raw readings from the sensor
     *
     */
    int sum_of_readings;

    /**
     * @brief
     * Number of raw readings from the sensor
     *
     */
    int number_of_readings;

    /**
     * @brief
     * Flag signaling the device is calibrating
     *
     */
    int calibrating : 1;

    /**
     * @brief
     * Step of calibration (No deposit, Full deposit)
     *
     */
    uint8_t step;
  } calibration;

  /**
   * @brief
   * Sensor raw value when the bottle is full
   *
   */
  int full_deposit;

  /**
   * @brief
   * Sensor raw value when the bottle is empty
   *
   */
  int no_deposit;

  /**
   * @brief
   * Current sensor raw value
   *
   */
  int current_deposit;

  /**
   * @brief
   * Current liters in the bottle
   *
   */
  int current_liters;

  /**
   * @brief
   * New raw value read from sensor, to compare to the saved one
   *
   */
  int new_reading;

  /**
   * @brief
   * Saved raw value from the sensor
   *
   */
  int old_reading;

  /**
   * @brief
   * Last raw value sent to AWS
   *
   */
  int last_sensor_sent;

  /**
   * @brief
   * Counter used to detect static value to re-send the value to aWS
   */
  uint8_t static_counter;

  /**
   * @brief
   * Flag signaling the sensor is stable
   *
   */
  int stable : 1;

  /**
   * @brief
   * Flag signaling there is no bottle, to calculate stock changes
   *
   */
  int no_bottle : 1;
};

/**
 * @brief
 * Smart ring main controller to group all the information on one place
 *
 */
typedef struct smart_ring_controller_t {
  /**
   * @brief
   * Device name to show on main screen header
   *
   */
  char device_name[32];

  /**
   * @brief
   * Device mac address to use on MQTT communication
   *
   */
  char mac_address[18];

  /**
   * @brief
   * Device connection main controller object
   *
   */
  struct smart_ring_connection_controller_t connection;

  /**
   * @brief
   * Device main flags
   *
   */
  union main_flags {
    struct smart_ring_flags_t flag;
    int has_flags;
  } flags;

  /**
   * @brief
   * Flag signaling that is the first time the device is booting up to request
   * the user to choose the communication method
   *
   */
  bool first_boot;

  /**
   * @brief
   * Sleep timer handle
   *
   */
  esp_timer_handle_t sleep_timer;

  /**
   * @brief
   * Sleep timeout in seconds
   *
   */
  int sleep_timeout;

  /**
   * @brief
   * Device stock
   *
   */
  int stock;

  /**
   * @brief
   * Device order mode
   *
   */
  char order_mode;

  /**
   * @brief
   * Sensor controller object
   *
   */
  struct smart_ring_sensor_t sensor;

  /**
   * @brief
   * UI controller object
   *
   */
  struct smart_ring_ui_controller_t *ui_controller;

  /**
   * @brief
   * Order object
   *
   */
  struct smart_ring_order_t order;

  /**
   * @brief
   * User name to be used when and if necessary (didn't see when)
   *
   */
  char user_name[32];

  /**
   * @brief
   * User role used to choose which options to show after logging in with pin
   *
   */
  char user_role;
};

/**
 * @brief
 * Get mac address from wifi interface and store it
 *
 * @return esp_err_t Result of retrieving mac address
 * @retval ESP_OK Mac address stored successfully
 * @retval Other Error getting mac address
 */
esp_err_t configure_mac_address(void);

/**
 * @brief
 * Get the device main controller pointer
 *
 * @return struct smart_ring_controller_t* - Main controller pointer
 */
struct smart_ring_controller_t *smart_ring_get_controller();

/**
 * @brief
 * Get device mac address
 *
 * @retval mac_address Device mac address
 */
char *smart_ring_get_mac_address();

/**
 * @brief
 * Set device name to be shown on UI
 *
 * @param device_name {String} - Device name
 */
void smart_ring_set_device_name(char *device_name);

/**
 * @brief
 * Get device name
 *
 * @retval device_name Device name
 */
char *smart_ring_get_device_name();

/**
 * @brief
 * Set user name
 *
 * @param user_name {String} - User name
 */
void smart_ring_set_user_name(char *user_name);

/**
 * @brief
 * Get user name
 *
 * @retval user_name User name
 */
char *smart_ring_get_user_name();

/**
 * @brief
 * Set user role
 *
 * @param user_role {char} - User role
 */
void smart_ring_set_user_role(char user_role);

/**
 * @brief
 * Get user role
 *
 * @retval m Master
 * @retval a Admin
 * @retval u User
 */
char smart_ring_get_user_role();

/**
 * @brief
 * Get device stock
 *
 * @retval Stock Device stock
 */
uint8_t smart_ring_get_stock();

/**
 * @brief
 * Set device stock
 *
 * @param stock {int} - Stock
 */
void smart_ring_set_stock(uint8_t stock);

/**
 * @brief
 * Set device order mode
 *
 * @param order_mode {char} - Order mode
 */
void smart_ring_set_order_mode(char order_mode);

/**
 * @brief
 * Get device order mode
 *
 * @retval m Manual
 * @retval a Automatic
 */
char smart_ring_get_order_mode();

#endif /* __VARS_H_ */