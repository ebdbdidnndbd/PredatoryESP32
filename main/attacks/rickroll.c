#include "esp_wifi.h"
#include "esp_http_server.h"
#include <string.h>

static esp_err_t rick_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "<html><meta http-equiv='refresh' content='0;url=https://www.youtube.com/watch?v=dQw4w9WgXcQ'></html>", 
                    HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void rickroll_start(void) {
    wifi_config_t ap_config = {0};
    strcpy((char *)ap_config.ap.ssid, "FREE_WIFI");
    ap_config.ap.ssid_len = 9;
    ap_config.ap.channel = 1;
    ap_config.ap.authmode = WIFI_AUTH_OPEN;
    
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
    
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &config);
    
    httpd_uri_t uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = rick_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &uri);
}
