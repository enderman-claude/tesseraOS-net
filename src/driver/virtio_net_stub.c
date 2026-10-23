/* virtio-net driver stub (N1). Every call reports NET_ERR_UNSUPPORTED. */
#include "net/driver.h"

static int stub_open(void *ctx) {
    (void)ctx;
    return NET_ERR_UNSUPPORTED;
}

static int stub_mac(void *ctx, net_mac *mac) {
    (void)ctx;
    (void)mac;
    return NET_ERR_UNSUPPORTED;
}

static int stub_send(void *ctx, const uint8_t *frame, size_t len) {
    (void)ctx;
    (void)frame;
    (void)len;
    return NET_ERR_UNSUPPORTED;
}

static int stub_recv(void *ctx, uint8_t *buf, size_t cap) {
    (void)ctx;
    (void)buf;
    (void)cap;
    return NET_ERR_UNSUPPORTED;
}

static const net_driver virtio_stub = {
    .name = "virtio-net (stub)",
    .open = stub_open,
    .mac = stub_mac,
    .send = stub_send,
    .recv = stub_recv,
    .ctx = 0,
};

const net_driver *net_driver_virtio(void) {
    return &virtio_stub;
}
