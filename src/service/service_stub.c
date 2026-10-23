/* Net service stub (N3 to N5). Socket calls report NET_ERR_UNSUPPORTED. */
#include <stdlib.h>

#include "net/service.h"

struct net_service {
    net_config cfg;
};

/* Config checks, so bad manifests fail at start-up rather than at first use. */
static int config_valid(const net_config *cfg) {
    switch (cfg->mode) {
    case NET_ADDR_STATIC:
        return cfg->static_ip != 0 && cfg->netmask != 0;
    case NET_ADDR_DHCP:
        return 1;
    case NET_ADDR_BOTH:
        return cfg->static_ip != 0 && cfg->netmask != 0;   /* fallback must be complete */
    default:
        return 0;
    }
}

int net_service_start(net_service **out, const net_config *cfg) {
    if (out == NULL || cfg == NULL || !config_valid(cfg)) {
        return NET_ERR_INVALID;
    }
    net_service *s = calloc(1, sizeof *s);
    if (s == NULL) {
        return NET_ERR_NOMEM;
    }
    s->cfg = *cfg;
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
