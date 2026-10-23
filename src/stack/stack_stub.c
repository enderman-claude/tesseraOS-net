/* IP stack stub (N2 and N3). Every call reports NET_ERR_UNSUPPORTED. */
#include <stdlib.h>

#include "net/stack.h"

struct net_stack {
    const net_driver *drv;
    net_ipv4 address;
};

int net_stack_init(net_stack **out, const net_driver *drv, net_ipv4 address) {
    if (drv == NULL || out == NULL) {
        return NET_ERR_INVALID;
    }
    net_stack *s = calloc(1, sizeof *s);
    if (s == NULL) {
        return NET_ERR_NOMEM;
    }
    s->drv = drv;
    s->address = address;
    *out = s;
    return NET_ERR_UNSUPPORTED;   /* stub: the stack is not running yet */
}

void net_stack_free(net_stack *s) {
    free(s);
}

int net_arp_handle(net_stack *s, const uint8_t *frame, size_t len) {
    (void)s; (void)frame; (void)len;
    return NET_ERR_UNSUPPORTED;
}

int net_icmp_ping(net_stack *s, net_ipv4 dst, uint32_t timeout_ms) {
    (void)s; (void)dst; (void)timeout_ms;
    return NET_ERR_UNSUPPORTED;
}

int net_udp_send(net_stack *s, const net_addr *dst, const uint8_t *data, size_t len) {
    (void)s; (void)dst; (void)data; (void)len;
    return NET_ERR_UNSUPPORTED;
}

int net_udp_recv(net_stack *s, uint16_t local_port, uint8_t *buf, size_t cap, net_addr *from) {
    (void)s; (void)local_port; (void)buf; (void)cap; (void)from;
    return NET_ERR_UNSUPPORTED;
}
