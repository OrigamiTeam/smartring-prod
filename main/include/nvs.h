/**
 * @file nvs.h
 * @authors  Ayinde Olayiwola (olay@quleap.com)
 * @brief Flash storage header
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
                         __         
                        /\ \        
  ___   __  __    ____  \ \ \___    
/' _ `\/\ \/\ \  /',__\  \ \  _ `\  
/\ \/\ \ \ \_/ |/\__, `\__\ \ \ \ \ 
\ \_\ \_\ \___/ \/\____/\_\\ \_\ \_\
 \/_/\/_/\/__/   \/___/\/_/ \/_/\/_/
                                                                      
 */

#ifndef __NVS_H_
#define __NVS_H_

/**
 * @brief
 * Save the wifi credentials received during provision
 *
 * ***
 *
 * ### Namespaces
 *
<table>
   <tr>
      <th>Variable</th>
      <th>NVS Namespace</th>
   </tr>
   <tr>
      <td style="text-align:center">SSID</td>
      <td style="text-align:center">ssid</td>
   </tr>
   <tr>
      <td style="text-align:center">Password</td>
      <td style="text-align:center">password</td>
   </tr>
</table>
 *
 * @param ssid {String} - Network SSID
 * @param password {String} - Network password
 * @param provisioned {boolean} - Device provisioned
 *
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_save_wifi_credentials(char *ssid, char *password, bool provisioned);

/**
 * @brief
 * Load the wifi credentials from the NVS storage space
 *
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_load_wifi_credentials(void);

/**
 * @brief
 * Delete the Wifi credentials namespaces from the NVS
 *
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_clear_wifi_credentials();

/**
 * @brief
 * Save the calibration information on the NVS
 *
 * ***
 *
 * ### Namespaces
 *
<table>
   <tr>
      <th>Variable</th>
      <th>NVS Namespace</th>
   </tr>
   <tr>
      <td style="text-align:center">No deposit</td>
      <td style="text-align:center">no_deposit</td>
   </tr>
   <tr>
      <td style="text-align:center">Full deposit</td>
      <td style="text-align:center">full_deposit</td>
   </tr>
   <tr>
      <td style="text-align:center">Device stable</td>
      <td style="text-align:center">is_stable</td>
   </tr>
   <tr>
      <td style="text-align:center">Stock</td>
      <td style="text-align:center">stock</td>
   </tr>
</table>
 *
 * @param no_deposit {int} - No deposit raw value
 * @param full_deposit {int} - Full deposit raw value
 * @param is_stable {boolean} - Sensor is stable
 * @param stock {int} - Stock
 *
 *
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_save_calibration(int no_deposit, int full_deposit, bool is_stable, int stock);

/**
 * @brief
 * Load the device calibration values from the NVS storage
 *
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_load_calibration(void);

/**
 * @brief
 * Save the device order mode on the NVS
 *
 * ***
 *
 * ### Namespaces
 *
<table>
   <tr>
      <th>Variable</th>
      <th>NVS Namespace</th>
   </tr>
   <tr>
      <td style="text-align:center">Order mode</td>
      <td style="text-align:center">order_mode</td>
   </tr>
</table>
 *
 * @param order_mode {char} - Device order mode
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_save_order_mode(char order_mode);

/**
 * @brief
 * Load device order mode from the NVS
 *
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_load_order_mode(void);

/**
 * @brief
 * Save the device connection type on the NVS
 *
 * @param connection_type {char} - Connection type
 *
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_save_connection_type(char connection_type);

/**
 * @brief
 * Load the device connection type from the NVS
 *
 * ***
 *
 * ### Namespaces
 *
<table>
   <tr>
      <th>Variable</th>
      <th>NVS Namespace</th>
   </tr>
   <tr>
      <td style="text-align:center">Connection type</td>
      <td style="text-align:center">connection_type</td>
   </tr>
</table>
 *
 * @return esp_err_t - Result of the operations on storage
 * @retval ESP_OK Credentials saved successfully
 * @retval Other Error on NVS
 */
esp_err_t nvs_load_connection_type(void);

esp_err_t nvs_load_temporary_mqtt_certificate_pem(char*) ;
esp_err_t nvs_load_temporary_mqtt_certificate_id(void);
esp_err_t nvs_save_mqtt_certificate_pem(char *pem) ;
esp_err_t nvs_save_mqtt_certificate_privateKey(char *prkey);
esp_err_t nvs_save_mqtt_certificate_Id(char *certificateId);
esp_err_t nvs_clear_certificate_credentials(void) ;


#endif