/* UPnP IGD stub (N7). Argument checks are real; the network calls are not written yet. */
#include "net/upnp.h"

int net_upnp_discover(char *control_url, size_t cap) {
    if (control_url == NULL || cap == 0) {
        return NET_ERR_INVALID;
    }
    return NET_ERR_UNSUPPORTED;
}

int net_upnp_add_mapping(const char *control_url, const net_mapping *m) {
    if (control_url == NULL || m == NULL || m->external_port == 0 || m->internal_ip == 0) {
        return NET_ERR_INVALID;
    }
    return NET_ERR_UNSUPPORTED;
}

int net_upnp_delete_mapping(const char *control_url, enum net_proto proto, uint16_t external_port) {
    (void)proto;
    if (control_url == NULL || external_port == 0) {
        return NET_ERR_INVALID;
    }
    return NET_ERR_UNSUPPORTED;
}

int net_upnp_external_ip(const char *control_url, net_ipv4 *out) {
    if (control_url == NULL || out == NULL) {
        return NET_ERR_INVALID;
    }
    return NET_ERR_UNSUPPORTED;
}
