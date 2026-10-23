/* Host tests for the networking stubs. Each stub must report NET_ERR_UNSUPPORTED (or
 * NET_ERR_INVALID for bad arguments). Exit 0 means every check passed. */
#include <stdio.h>

#include "net/driver.h"
#include "net/service.h"
#include "net/stack.h"

static int failures;

#define CHECK(cond, msg)                                                  \
    do {                                                                  \
        if (!(cond)) {                                                    \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++;                                                   \
        }                                                                 \
    } while (0)

static void test_driver_stub(void) {
    const net_driver *d = net_driver_virtio();
    CHECK(d != NULL && d->name != NULL, "driver table present");
    net_mac mac;
    uint8_t frame[64] = { 0 };
    uint8_t buf[NET_MAX_FRAME];
    CHECK(d->open(d->ctx) == NET_ERR_UNSUPPORTED, "driver open is a stub");
    CHECK(d->mac(d->ctx, &mac) == NET_ERR_UNSUPPORTED, "driver mac is a stub");
    CHECK(d->send(d->ctx, frame, sizeof frame) == NET_ERR_UNSUPPORTED, "driver send is a stub");
    CHECK(d->recv(d->ctx, buf, sizeof buf) == NET_ERR_UNSUPPORTED, "driver recv is a stub");
}

static void test_stack_stub(void) {
    net_stack *s = NULL;
    const net_driver *d = net_driver_virtio();
    CHECK(net_stack_init(&s, NULL, 0) == NET_ERR_INVALID, "stack init rejects a null driver");
    CHECK(net_stack_init(&s, d, 0x0A00020Fu) == NET_ERR_UNSUPPORTED, "stack init is a stub");
    CHECK(s != NULL, "stack handle still allocated, so teardown can be tested");
    CHECK(net_arp_handle(s, NULL, 0) == NET_ERR_UNSUPPORTED, "arp is a stub");
    CHECK(net_icmp_ping(s, 0x08080808u, 1000) == NET_ERR_UNSUPPORTED, "icmp is a stub");
    net_addr dst = { 0x08080808u, 53 };
    uint8_t data[4] = { 1, 2, 3, 4 };
    CHECK(net_udp_send(s, &dst, data, sizeof data) == NET_ERR_UNSUPPORTED, "udp send is a stub");
    CHECK(net_udp_recv(s, 53, data, sizeof data, &dst) == NET_ERR_UNSUPPORTED, "udp recv is a stub");
    net_stack_free(s);
}

static void test_service_stub(void) {
    net_service *svc = NULL;
    CHECK(net_service_start(&svc, 0x0A00020Fu) == NET_ERR_UNSUPPORTED, "service start is a stub");
    CHECK(svc != NULL, "service handle still allocated, so teardown can be tested");
    net_addr a = { 0x0A000001u, 7 };
    uint8_t buf[8] = { 0 };
    CHECK(net_socket(svc, NET_AF_INET, NET_SOCK_DGRAM) == NET_ERR_UNSUPPORTED, "socket is a stub");
    CHECK(net_bind(svc, 0, &a) == NET_ERR_UNSUPPORTED, "bind is a stub");
    CHECK(net_connect(svc, 0, &a) == NET_ERR_UNSUPPORTED, "connect is a stub");
    CHECK(net_send(svc, 0, buf, sizeof buf) == NET_ERR_UNSUPPORTED, "send is a stub");
    CHECK(net_recv(svc, 0, buf, sizeof buf) == NET_ERR_UNSUPPORTED, "recv is a stub");
    CHECK(net_close(svc, 0) == NET_ERR_UNSUPPORTED, "close is a stub");
    net_service_stop(svc);
}

int main(void) {
    test_driver_stub();
    test_stack_stub();
    test_service_stub();
    if (failures) {
        fprintf(stderr, "test_stubs: %d failure(s)\n", failures);
        return 1;
    }
    printf("test_stubs: all checks passed\n");
    return 0;
}
