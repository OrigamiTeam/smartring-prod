/**
 * @file sensors.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief Sensors functions
 * @version 2.1.2
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "sensors.h"
#include "sntp.h"
#include "sntpprotocol.h"

static const char *TAG = "SENSOR";

static TickType_t lastTimePercCheck = 0;

static int smart_ring_sensors_read(void)
{
  adc_update();
  int raw_value = adc_getData();

#ifndef NDEBUG
  ESP_LOGI(TAG, "Load cell output : %d", raw_value);
#endif

  return raw_value;
}

/**
 * @brief
 * Validate the value read from the sensor, compare to the previously saved and
 * update the UI and AWS if needed
 *
 * @param controller - Main system controller pointer
 * @param sensor_value - Raw sensor value
 */

static void
smart_ring_sensors_validate(struct smart_ring_controller_t *controller, int sensor_value)
{

  controller->sensor.old_reading = controller->sensor.new_reading;
  controller->sensor.new_reading = sensor_value;

  // Separate if the device is calibrating or not
  if (controller->sensor.calibration.calibrating)
  {
    if (controller->sensor.calibration.timestamp != -1)
    {
#ifndef NDEBUG
      ESP_LOGI(TAG, "\n\tCurrent time : %lld - %.2f\n\tLast saved : %ld - %.2f",
               esp_timer_get_time(), (float)esp_timer_get_time() / 1000000.0,
               controller->sensor.calibration.timestamp,
               (float)controller->sensor.calibration.timestamp / 1000000.0);
#endif
      ESP_LOGI(TAG, "Calibrating % d sec stabilized",
               (int)((esp_timer_get_time() - controller->sensor.calibration.timestamp) / 1000000));
    }
    else
    {
      return;
    }

    if ((controller->sensor.new_reading - controller->sensor.old_reading >= -STABILIZED_INTERVAL) &&
        (controller->sensor.new_reading - controller->sensor.old_reading <= STABILIZED_INTERVAL))
    {

      if ((int)((esp_timer_get_time() - controller->sensor.calibration.timestamp) / 1000000) > 9)
      {
        smart_ring_ui_update_state(STATE_31);
        controller->ui_controller->calibration_step = controller->sensor.calibration.step;

        switch (controller->sensor.calibration.step)
        {
        case 1:
          controller->sensor.no_deposit = controller->sensor.calibration.sum_of_readings /
                                          controller->sensor.calibration.number_of_readings;

#ifndef NDEBUG
          ESP_LOGI(TAG, "Empty deposit value : %d", controller->sensor.no_deposit);
#endif
          controller->sensor.calibration.timestamp = esp_timer_get_time();
          break;

        case 2:
          controller->sensor.full_deposit = controller->sensor.calibration.sum_of_readings /
                                            controller->sensor.calibration.number_of_readings;
#ifndef NDEBUG
          ESP_LOGI(TAG, "Full deposit value : %d", controller->sensor.full_deposit);
#endif
          controller->sensor.old_reading = controller->sensor.no_deposit;
          controller->sensor.stable = true;
          nvs_save_calibration(controller->sensor.no_deposit, controller->sensor.full_deposit,
                               controller->sensor.stable, controller->stock);
          mqtt_send_message(SEND_CALIBRATION);
          controller->sensor.no_bottle = false;
          break;

        default:
          ESP_LOGE(TAG, "Invalid calibration step : %d", controller->sensor.calibration.step);
          break;
        }
        controller->sensor.calibration.sum_of_readings = 0;
        controller->sensor.calibration.number_of_readings = 0;
        controller->sensor.calibration.timestamp = -1;
        controller->sensor.calibration.calibrating = false;
      }
      else
      {
        controller->sensor.calibration.sum_of_readings += sensor_value;
        controller->sensor.calibration.number_of_readings++;
      }
    }
    else
    {
      controller->sensor.calibration.sum_of_readings = 0;
      controller->sensor.calibration.number_of_readings = 0;
      controller->sensor.calibration.timestamp = esp_timer_get_time();
    }
  }
  else
  {
    smart_ring_ui_set_calibration(controller->sensor.no_deposit,
                                  controller->sensor.full_deposit,
                                  controller->sensor.stable);

    // If on debug screen change the value in real time
    if (controller->ui_controller->state == STATE_28 && !controller->ui_controller->flags.flag.update_state)
    {
      smart_ring_main_debug_update_sensor_values(controller->sensor.new_reading, controller->sensor.old_reading,
                                                 controller->sensor.stable);
    }

    // Check if the consumption is greater 0.3litres
    if ((abs(controller->sensor.new_reading - controller->sensor.last_sensor_sent) >
         ((controller->sensor.full_deposit - controller->sensor.no_deposit) / FULL_LITRES) / DIVISION_LITRES))
    {

      if (++controller->sensor.static_counter >= STATIC_MAX_COUNTER)
      {
        int percent_value = map(controller->sensor.new_reading, controller->sensor.no_deposit,
                                controller->sensor.full_deposit, 0, 100);
        ESP_LOGI(TAG, "Sensor percent value: %d %%", percent_value);
        ESP_LOGI(TAG, "Sensor value: %.2f liters",
                 (controller->sensor.new_reading - controller->sensor.no_deposit) * FULL_LITRES /
                     (controller->sensor.full_deposit - controller->sensor.no_deposit));

        if (percent_value > 75)
        {
          controller->sensor.current_liters = 18900.0;
          percent_value = 100;
          ESP_LOGI(TAG, "Percent: %d", percent_value);
          ESP_LOGI(TAG, "Liters: %d", controller->sensor.current_liters);
          if (controller->sensor.no_bottle && (controller->sensor.calibration.step == 0))
          {
            controller->stock > 0 ? controller->stock-- : NULL;
            controller->ui_controller->stock = controller->stock;
            if (controller->ui_controller->state == STATE_5 && !controller->ui_controller->flags.flag.update_state)
            {
              smart_ring_ui_main_update_stock_value(controller->stock);
              nvs_save_calibration(controller->sensor.no_deposit, controller->sensor.full_deposit,
                                   controller->sensor.stable, controller->stock);
              controller->ui_controller->updated_stock = controller->stock;
              controller->ui_controller->flags.flag.update_stock_manual = true;
            }
#ifndef NDEBUG
            ESP_LOGI(TAG, "Send stock to MQTT");
#endif
            //mqtt_send_message(SEND_STOCK);
            mqtt_send_message(SEND_ALERT_CHNGGALLON);
            controller->sensor.no_bottle = false;
          }
        }
        else if (percent_value > 50)
        {
          controller->sensor.current_liters = 14180.0;
          percent_value = 75;
          ESP_LOGI(TAG, "Percent: %d", percent_value);
          ESP_LOGI(TAG, "Liters: %d", controller->sensor.current_liters);
          controller->sensor.no_bottle = false;
        }
        else if (percent_value > 25)
        {
          controller->sensor.current_liters = 9450.0;
          percent_value = 50;
          ESP_LOGI(TAG, "Percent: %d", percent_value);
          ESP_LOGI(TAG, "Liters: %d", controller->sensor.current_liters);
          controller->sensor.no_bottle = false;
        }
        else
        {
          controller->sensor.current_liters = 4730.0;
          percent_value = 25;
          ESP_LOGI(TAG, "Percent: %d", percent_value);
          ESP_LOGI(TAG, "Liters: %d", controller->sensor.current_liters);
          ESP_LOGI(TAG, "No Deposit");
          controller->sensor.no_bottle = true;
        }

        smart_ring_ui_set_current_deposit(controller->sensor.new_reading);

        // mqtt_send_message(SEND_SENSORS);
#ifndef NDEBUG
        ESP_LOGI(TAG, "Send sensor to MQTT");
#endif
        controller->sensor.last_sensor_sent = controller->sensor.new_reading;
        controller->sensor.static_counter = 0;
        controller->sensor.stable = true;

        // The sensor value is stable
        controller->sensor.current_deposit = controller->sensor.new_reading;
        if (controller->ui_controller->state == STATE_5 &&
            !controller->ui_controller->flags.flag.update_state)
        {
          // printf("New Reading : %d \n",  controller->sensor.new_reading );
          // smart_ring_ui_set_current_deposit(controller->sensor.new_reading);
        }

        if (controller->sensor.stable == false)
        {
          // mqtt_send_message(SEND_SENSORS);
#ifndef NDEBUG
          ESP_LOGI(TAG, "Send sensor to MQTT");
#endif
          if (controller->ui_controller->state == STATE_28 &&
              !controller->ui_controller->flags.flag.update_state)
          {
            smart_ring_main_debug_update_sensor_values(NO_UPDATE, NO_UPDATE, controller->sensor.stable);
          }
        }
      }
    }
    else
    {
      if (controller->sensor.stable)
      {
        controller->sensor.stable = false;
        controller->sensor.static_counter = 0;

        if (controller->ui_controller->state == STATE_28 && !controller->ui_controller->flags.flag.update_state)
        {
          smart_ring_main_debug_update_sensor_values(NO_UPDATE, NO_UPDATE, controller->sensor.stable);
        }
      }
    }
  }
}

void smart_ring_sensors_task(void *pvParameter)
{

  struct smart_ring_controller_t *controller = smart_ring_get_controller();

  ESP_LOGI(TAG, "Set Up Sensors...");
  adc_begin(4, 2);         //(dout pin, sck pin)
  adc_setCalFactor(100.0); // user set calibration factor (float)
  ESP_LOGI(TAG, "Done");

  for (;;)
  {
    smart_ring_sensors_validate(controller, smart_ring_sensors_read());
    if (controller->ui_controller->flags.flag.register_water_level)
    {
      //mqtt_send_message(SEND_SENSORS);
      controller->ui_controller->flags.flag.register_water_level = false;
    }

    if (xTaskGetTickCount() - lastTimePercCheck >= TIME_PERC_INTERVAL)
    {
      ESP_LOGI("SNTP", "Check Time Percentage");

      if (isTimePerc(controller))
      {
        ESP_LOGI("SNTP", "Send Percentual to MQTT");
        mqtt_send_message(SEND_PERCENTUAL);
        controller->flags.flag.sent_perc_message = true;
      }

      lastTimePercCheck = xTaskGetTickCount(); // Update the last checked time
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}

/* END OF FILE */