/* Net service stub (N3 to N5). Socket calls report NET_ERR_UNSUPPORTED. */
#include <stdlib.h>

#include "net/service.h"

struct net_service {
    net_ipv4 address;
};

int net_service_start(net_service **out, net_ipv4 address) {
    if (out == NULL) {
        return NET_ERR_INVALID;
    }
    net_service *s = calloc(1, sizeof *s);
    if (s == NULL) {
        return NET_ERR_NOMEM;
    }
    s->address = address;
    *out = s;
    return NET_ERR_UNSUPPORTED;   /* stub: no driver is started yet */
}

void net_service_stop(net_service *s) {
    free(s);
}

int net_socket(net_service *s, int family, int type) {
    (void)s; (void)family; (void)type;
    return NET_ERR_UNSUPPORTED;
}

int net_bind(net_service *s, int sock, const net_addr *local) {
    (void)s; (void)sock; (void)local;
    return NET_ERR_UNSUPPORTED;
}

int net_connect(net_service *s, int sock, const net_addr *remote) {
    (void)s; (void)sock; (void)remote;
    return NET_ERR_UNSUPPORTED;
}

int64_t net_send(net_service *s, int sock, const uint8_t *data, size_t len) {
    (void)s; (void)sock; (void)data; (void)len;
    return NET_ERR_UNSUPPORTED;
}

int64_t net_recv(net_service *s, int sock, uint8_t *buf, size_t cap) {
    (void)s; (void)sock; (void)buf; (void)cap;
    return NET_ERR_UNSUPPORTED;
}

int net_close(net_service *s, int sock) {
    (void)s; (void)sock;
    return NET_ERR_UNSUPPORTED;
}
