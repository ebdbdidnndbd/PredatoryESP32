#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"

void deauth_attack(uint8_t *bssid);
void beacon_flood(char *ssid);
void start_sniffing(void);
void evil_twin_start(char *target_ssid);
void rickroll_start(void);

void attack_loop(void *pv) {
    while(1) {
        uint8_t bssid[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        deauth_attack(bssid);
        beacon_flood("FreeWiFi");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_promiscuous(true);
    
    xTaskCreate(attack_loop, "attack_loop", 4096, NULL, 5, NULL);
}
