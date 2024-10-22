/**
 * @file sntp.c
 * @author Mateus Araujo @ Quantum Leap (mateus@quleap.com)
 * @brief
 * This file contains the functions responsible for the Simple Network Time Protocol (SNTP)
 *
 * @version 2.1.2
 * @date 2024-04-09
 *
 * @copyright Copyright (c) 2024
 */

#include "libs.h"
#include "sntp.h"

static const char *TAG = "SNTP";

void time_sync_notification_cb(struct timeval *tv){
    ESP_LOGI(TAG, "Time sync from NTP server");
}

void initialize_sntp(){
    ESP_LOGI(TAG, "Initializing SNTP");
    
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    
    sntp_init();
}

void wait_for_sntp_sync() {
    int retry = 0;
    const int retry_count = 5;  // Maximum number of retries
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void set_timezone() {
    // Set timezone to Western European Time (WET) and Western European Summer Time (WEST)
    // WET is UTC+0 and WEST (summer time) is UTC+1
    setenv("TZ", "WET-0WEST,M3.5.0/1,M10.5.0/2", 1);
    
    ESP_LOGI(TAG, "Timezone set");
    tzset();
}

bool isTimePerc(struct smart_ring_controller_t *controller){
    time_t now;
    struct tm timeinfo;
    char buffer[64];

    time(&now);
    localtime_r(&now, &timeinfo);

    if (timeinfo.tm_min != 0) {
        controller->flags.flag.sent_perc_message = false;
    }
    
    if ((timeinfo.tm_hour == 1  ||
         timeinfo.tm_hour == 6  ||
         timeinfo.tm_hour == 11 ||
         timeinfo.tm_hour == 17 || 
         timeinfo.tm_hour == 22  ) && 
         timeinfo.tm_min  == 0     && !controller->flags.flag.sent_perc_message) {
        return true;
    }
    return false;
}

// Version Check Function
// Verifies if time equals 8h30
//     If true makes a firmware version check
bool versionCheck(struct smart_ring_controller_t *controller){
    time_t now;
    struct tm timeinfo;
    char buffer[64];

    time(&now);
    localtime_r(&now, &timeinfo);

    if (timeinfo.tm_min != 30) {
        controller->flags.flag.sent_version_check_message = false;
    }
    
    if (timeinfo.tm_hour == 8 && timeinfo.tm_min  == 30 && !controller->flags.flag.sent_version_check_message) {
        return true;
    }
    return false;
}