/* Wi-Fi driver stub. Specific hardware is out of scope for now (decision 4): this exists so the
 * driver interface covers a wireless device. Every call reports NET_ERR_UNSUPPORTED. */
#include "net/driver.h"

static int wifi_open(void *ctx) {
    (void)ctx;
    return NET_ERR_UNSUPPORTED;
}

static int wifi_mac(void *ctx, net_mac *mac) {
    (void)ctx;
    (void)mac;
    return NET_ERR_UNSUPPORTED;
}

static int wifi_send(void *ctx, const uint8_t *frame, size_t len) {
    (void)ctx;
    (void)frame;
    (void)len;
    return NET_ERR_UNSUPPORTED;
}

static int wifi_recv(void *ctx, uint8_t *buf, size_t cap) {
    (void)ctx;
    (void)buf;
    (void)cap;
    return NET_ERR_UNSUPPORTED;
}

static const net_driver wifi_stub_driver = {
    .name = "wifi (stub, no chipset supported)",
    .open = wifi_open,
    .mac = wifi_mac,
    .send = wifi_send,
    .recv = wifi_recv,
    .ctx = 0,
};

const net_driver *net_driver_wifi_stub(void) {
    return &wifi_stub_driver;
}
