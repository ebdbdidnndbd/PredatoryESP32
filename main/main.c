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

// صفحة الويب الرئيسية
static esp_err_t index_handler(httpd_req_t *req) {
    const char *html = 
        "<!DOCTYPE html><html><head><title>Predatory ESP32</title>"
        "<style>body{background:#0a0a0a;color:#00ff41;font-family:monospace;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}"
        ".container{text-align:center;border:2px solid #00ff41;padding:40px;border-radius:12px;background:#111;}"
        "h1{font-size:2.5em;margin-bottom:30px;text-shadow:0 0 20px #00ff41;}"
        ".grid{display:grid;grid-template-columns:1fr 1fr;gap:15px;}"
        "button{background:transparent;border:2px solid #00ff41;color:#00ff41;padding:15px 25px;font-size:1.2em;cursor:pointer;border-radius:8px;}"
        "button:hover{background:#00ff41;color:#0a0a0a;box-shadow:0 0 30px #00ff41;}"
        "#status{margin-top:25px;font-size:1.2em;padding:10px;border:1px solid #00ff41;border-radius:6px;}"
        "</style></head><body>"
        "<div class='container'>"
        "<h1>⚡ Predatory ESP32</h1>"
        "<div class='grid'>"
        "<button onclick=\"fetch('/start')\">▶ START</button>"
        "<button onclick=\"fetch('/stop')\">⏹ STOP</button>"
        "<button onclick=\"fetch('/deauth')\">⚡ Deauth</button>"
        "<button onclick=\"fetch('/beacon')\">📡 Beacon</button>"
        "</div>"
        "<div id='status'>✅ System Ready</div>"
        "</div>"
        "<script>function fetch(url){ fetch(url).then(() => { document.getElementById('status').textContent = '✅ ' + url; }); }</script>"
        "</body></html>";
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, strlen(html));
    return ESP_OK;
}

// معالج الأوامر
static esp_err_t api_handler(httpd_req_t *req) {
    const char *uri = req->uri;
    ESP_LOGI(TAG, "API: %s", uri);
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

// تشغيل خادم الويب
void start_web_server(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    
    ESP_LOGI(TAG, "Starting web server...");
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t index = { .uri = "/", .method = HTTP_GET, .handler = index_handler };
        httpd_register_uri_handler(server, &index);
        httpd_uri_t api = { .uri = "/*", .method = HTTP_GET, .handler = api_handler };
        httpd_register_uri_handler(server, &api);
        ESP_LOGI(TAG, "✅ Web server started!");
    } else {
        ESP_LOGE(TAG, "❌ Web server failed!");
    }
}

// تشغيل نقطة الوصول
void start_ap(void) {
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "FREE_WIFI",
            .ssid_len = 9,
            .password = "",
            .channel = 1,
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 4,
        },
    };
    ESP_LOGI(TAG, "Starting AP...");
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
    ESP_LOGI(TAG, "✅ AP started: FREE_WIFI");
}

// الدالة الرئيسية
void app_main(void) {
    ESP_LOGI(TAG, "=== BOOTING ===");
    
    // NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    
    // الشبكة
    esp_netif_init();
    esp_event_loop_create_default();
    
    // WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    
    // AP
    start_ap();
    
    // Web Server
    start_web_server();
    
    ESP_LOGI(TAG, "=== ✅ SYSTEM READY ===");
    ESP_LOGI(TAG, "Connect to FREE_WIFI and open http://192.168.4.1");
}
