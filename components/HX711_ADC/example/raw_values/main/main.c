#include "HX711_ADC.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <string.h>

long t = 0; // aux println sensors data

//***********MILLIS***************************************
unsigned long IRAM_ATTR t_millis() {
    return (unsigned long)(esp_timer_get_time() / 1000ULL);
}
//********************************************************

void adc_task(void *pvParameter) {
    // adc_setPins(2, 4); //(dout pin, sck pin)
    printf("Wait...\n");
    adc_begin(2, 4);         //(dout pin, sck pin)
    adc_setCalFactor(500.0); // user set calibration factor (float)
    printf("Startup\n");

    for (;;) {
        // update() should be called at least as often as HX711 sample rate;
        // >10Hz@10SPS, >80Hz@80SPS longer delay in scetch will reduce effective
        // sample rate (be carefull with delay() in loop)
        adc_update();

        // get smoothed value from data set + current calibration factor
        if (t_millis() > t + 200) {
            int i = adc_getData();
            printf("Load_cell output val: %d\n", i);
            t = t_millis();
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    xTaskCreate(&adc_task, "adc_task", 1024, NULL, 5, NULL);
}
