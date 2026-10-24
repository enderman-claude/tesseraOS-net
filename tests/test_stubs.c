/* Host tests for the networking stubs. Each stub reports NET_ERR_UNSUPPORTED, or
 * NET_ERR_INVALID for bad arguments. Config validation is real code, so it is tested fully.
 * Exit 0 means every check passed. */
#include <stdio.h>

#include "net/driver.h"
#include "net/upnp.h"
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

static void check_driver_stubs(const net_driver *d, const char *which) {
    net_mac mac;
    uint8_t frame[64] = { 0 };
    uint8_t buf[NET_MAX_FRAME];
    CHECK(d != NULL && d->name != NULL, which);
    CHECK(d->open(d->ctx) == NET_ERR_UNSUPPORTED, which);
    CHECK(d->mac(d->ctx, &mac) == NET_ERR_UNSUPPORTED, which);
    CHECK(d->send(d->ctx, frame, sizeof frame) == NET_ERR_UNSUPPORTED, which);
    CHECK(d->recv(d->ctx, buf, sizeof buf) == NET_ERR_UNSUPPORTED, which);
}

static void test_driver_stubs(void) {
    check_driver_stubs(net_driver_virtio(), "virtio (generic) driver stub reports unsupported");
    check_driver_stubs(net_driver_wifi_stub(), "wifi driver stub reports unsupported");
}

static void test_stack_stub(void) {
    net_stack *s = NULL;
    const net_driver *d = net_driver_virtio();
    CHECK(net_stack_init(&s, NULL, 0) == NET_ERR_INVALID, "stack init rejects a null driver");
    CHECK(net_stack_init(&s, d, 0x0A00020Fu) == NET_ERR_UNSUPPORTED, "stack init is a stub");
    CHECK(s != NULL, "stack handle still allocated, so teardown can be tested");
    CHECK(net_arp_handle(s, NULL, 0) == NET_ERR_UNSUPPORTED, "arp is a stub");
    CHECK(net_icmp_ping(s, 0x08080808u, 1000) == NET_ERR_UNSUPPORTED, "icmp is a stub");
    net_addr dst = net_addr_v4(0x08080808u, 53);
    uint8_t data[4] = { 1, 2, 3, 4 };
    CHECK(net_udp_send(s, &dst, data, sizeof data) == NET_ERR_UNSUPPORTED, "udp send is a stub");
    CHECK(net_udp_recv(s, 53, data, sizeof data, &dst) == NET_ERR_UNSUPPORTED, "udp recv is a stub");
    net_stack_free(s);
}

static void test_config_validation(void) {
    net_service *svc = NULL;
    net_config cfg;

    /* DHCP needs no static fields. */
    cfg = (net_config){ NET_ADDR_DHCP, 0, 0, 0 };
    CHECK(net_service_start(&svc, &cfg) == NET_ERR_UNSUPPORTED, "DHCP config accepted (stub)");
    CHECK(svc != NULL, "service allocated for DHCP");
    net_service_stop(svc);
    svc = NULL;

    /* Static needs an address and a netmask. */
    cfg = (net_config){ NET_ADDR_STATIC, 0x0A00020Fu, 0xFFFFFF00u, 0x0A000201u };
    CHECK(net_service_start(&svc, &cfg) == NET_ERR_UNSUPPORTED, "static config accepted (stub)");
    net_service_stop(svc);
    svc = NULL;
    cfg = (net_config){ NET_ADDR_STATIC, 0, 0xFFFFFF00u, 0 };
    CHECK(net_service_start(&svc, &cfg) == NET_ERR_INVALID, "static without address rejected");
    CHECK(svc == NULL, "nothing allocated for a rejected config");
    cfg = (net_config){ NET_ADDR_STATIC, 0x0A00020Fu, 0, 0 };
    CHECK(net_service_start(&svc, &cfg) == NET_ERR_INVALID, "static without netmask rejected");

    /* Both: DHCP first, so the static fallback must be complete. */
    cfg = (net_config){ NET_ADDR_BOTH, 0x0A00020Fu, 0xFFFFFF00u, 0x0A000201u };
    CHECK(net_service_start(&svc, &cfg) == NET_ERR_UNSUPPORTED, "both config accepted (stub)");
    net_service_stop(svc);
    svc = NULL;
    cfg = (net_config){ NET_ADDR_BOTH, 0, 0, 0 };
    CHECK(net_service_start(&svc, &cfg) == NET_ERR_INVALID, "both without static fallback rejected");

    /* Unknown mode and null arguments. */
    cfg = (net_config){ 99, 0, 0, 0 };
    CHECK(net_service_start(&svc, &cfg) == NET_ERR_INVALID, "unknown mode rejected");
    CHECK(net_service_start(&svc, NULL) == NET_ERR_INVALID, "null config rejected");
    CHECK(net_service_start(NULL, &(net_config){ NET_ADDR_DHCP, 0, 0, 0 }) == NET_ERR_INVALID,
          "null out pointer rejected");
}

static void test_socket_stubs(void) {
    net_service *svc = NULL;
    net_config cfg = { NET_ADDR_DHCP, 0, 0, 0 };
    CHECK(net_service_start(&svc, &cfg) == NET_ERR_UNSUPPORTED, "service start is a stub");
    CHECK(svc != NULL, "service handle still allocated, so teardown can be tested");
    net_addr a = net_addr_v4(0x0A000001u, 7);
    uint8_t buf[8] = { 0 };
    CHECK(net_socket(svc, NET_AF_INET, NET_SOCK_DGRAM) == NET_ERR_UNSUPPORTED, "socket is a stub");
    CHECK(net_bind(svc, 0, &a) == NET_ERR_UNSUPPORTED, "bind is a stub");
    CHECK(net_connect(svc, 0, &a) == NET_ERR_UNSUPPORTED, "connect is a stub");
    CHECK(net_send(svc, 0, buf, sizeof buf) == NET_ERR_UNSUPPORTED, "send is a stub");
    CHECK(net_recv(svc, 0, buf, sizeof buf) == NET_ERR_UNSUPPORTED, "recv is a stub");
    CHECK(net_close(svc, 0) == NET_ERR_UNSUPPORTED, "close is a stub");
    net_service_stop(svc);
}

static void test_addresses_and_ports(void) {
    net_ipv6 loop = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } };
    net_addr v6 = net_addr_v6(&loop, 443);
    CHECK(v6.ip.family == NET_AF_INET6, "IPv6 address keeps its family");
    CHECK(v6.ip.u.v6.octet[15] == 1 && v6.port == 443, "IPv6 address and port are stored");
    net_addr v4 = net_addr_v4(0x0A000001u, 80);
    CHECK(v4.ip.family == NET_AF_INET && v4.ip.u.v4 == 0x0A000001u, "IPv4 address keeps its family");
    CHECK(!net_port_is_ephemeral(80), "port 80 is not ephemeral");
    CHECK(!net_port_is_ephemeral(49151), "49151 is below the dynamic range");
    CHECK(net_port_is_ephemeral(49152), "49152 starts the dynamic range");
    CHECK(net_port_is_ephemeral(65535), "65535 ends the dynamic range");
}

static void test_upnp_stubs(void) {
    char url[64];
    net_mapping m = { NET_PROTO_TCP, 8080, 0x0A00020Fu, 8080, 3600, "web" };
    net_ipv4 ext;
    CHECK(net_upnp_discover(NULL, 64) == NET_ERR_INVALID, "discover rejects a null buffer");
    CHECK(net_upnp_discover(url, sizeof url) == NET_ERR_UNSUPPORTED, "discover is a stub");
    CHECK(net_upnp_add_mapping(url, &m) == NET_ERR_UNSUPPORTED, "add mapping is a stub");
    m.external_port = 0;
    CHECK(net_upnp_add_mapping(url, &m) == NET_ERR_INVALID, "external port 0 rejected");
    CHECK(net_upnp_delete_mapping(url, NET_PROTO_UDP, 0) == NET_ERR_INVALID, "delete port 0 rejected");
    CHECK(net_upnp_delete_mapping(url, NET_PROTO_UDP, 53) == NET_ERR_UNSUPPORTED, "delete is a stub");
    CHECK(net_upnp_external_ip(url, &ext) == NET_ERR_UNSUPPORTED, "external ip is a stub");
}

int main(void) {
    test_addresses_and_ports();
    test_upnp_stubs();
    test_driver_stubs();
    test_stack_stub();
    test_config_validation();
    test_socket_stubs();
    if (failures) {
        fprintf(stderr, "test_stubs: %d failure(s)\n", failures);
        return 1;
    }
    printf("test_stubs: all checks passed\n");
    return 0;
}
