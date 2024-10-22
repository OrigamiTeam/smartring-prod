/**
 * @file spiffs.c
 * @author Ayinde Olayiwola @ Quantum Leap (olay@quleap.com)
 * @brief
 * This file contains the functions responsible for the initialization and
 * update of the SPIFFs storage portion of the device
 *
 *
 * @version -
 * @date 2023-05-03
 *
 * @copyright Copyright (c) 2023
 *
 */


#include "libs.h"

void spiffs_save_mqtt_certificate_privateKey(char*);
void spiffs_load_mqtt_certificate_privateKey(char*);


spiffs_t spiffs = {
        .saveMQTTCertificatePrivateKey = spiffs_save_mqtt_certificate_privateKey,
        .loadMQTTCertificatePrivateKey = spiffs_load_mqtt_certificate_privateKey,
};


static const char *TAG = "spiffs";


void spiffs_save_mqtt_certificate_privateKey(char *key) {
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/mqtt",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_spiffs_register(&config);

    /*Create file with name hello.txt */
    ESP_LOGI(TAG, "Creating New file: key.txt");
    FILE *f = fopen("/mqtt/key.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    ESP_LOGI(TAG, "Writing data to file: key.txt");
    fprintf(f, key);  // write data to pem.txt file
    fclose(f);
    ESP_LOGI(TAG, "File written");
}



void spiffs_load_mqtt_certificate_privateKey(char* key) {
     esp_vfs_spiffs_conf_t config = {
      .base_path              = "/mqtt",
      .partition_label        = NULL,
      .max_files              = 5,
      .format_if_mount_failed = true,
    };

    char tempString[100];
    esp_vfs_spiffs_register(&config);

    /* read data from key.txt file */
    ESP_LOGI(TAG, "Reading data from file: key.txt");
    FILE *file = fopen("/mqtt/key.txt", "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "File does not exist!");
    }
    else {
        while (fgets(tempString, sizeof(tempString), file) != NULL) {
            strcat(key, tempString);
        }
        fclose(file);
    }
    esp_vfs_spiffs_unregister(NULL);
}
