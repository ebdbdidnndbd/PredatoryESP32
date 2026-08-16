#include "esp_wifi.h"
#include "esp_log.h"

static const char *TAG = "SNIFF";

void probe_callback(void *buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    uint8_t *data = pkt->payload;
    if (data[0] == 0x40) {
        ESP_LOGI(TAG, "Probe: %02X:%02X:%02X:%02X:%02X:%02X",
                 data[10], data[11], data[12], data[13], data[14], data[15]);
    }
}

void start_sniffing(void) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(probe_callback);
    ESP_LOGI(TAG, "Sniffing started!");
}
