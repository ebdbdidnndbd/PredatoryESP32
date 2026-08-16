#include "esp_wifi.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "RICKROLL";

void rickroll_start(void) {
    wifi_config_t ap_config = {0};
    strcpy((char *)ap_config.ap.ssid, "FREE_WIFI");
    ap_config.ap.ssid_len = 9;
    ap_config.ap.channel = 1;
    ap_config.ap.authmode = WIFI_AUTH_OPEN;
    
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
    
    ESP_LOGI(TAG, "Rickroll AP started!");
}
