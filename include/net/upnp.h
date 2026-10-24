/* UPnP Internet Gateway Device port mapping: SSDP discovery, then SOAP calls on the router's
 * WANIPConnection service. Stubs until milestone N7. NAT-PMP (RFC 6886) and PCP (RFC 6887) are
 * the alternatives if a router lacks UPnP; not planned yet. */
#ifndef NET_UPNP_H
#define NET_UPNP_H

#include "net/net.h"

enum net_proto { NET_PROTO_TCP = 6, NET_PROTO_UDP = 17 };

typedef struct net_mapping {
    enum net_proto proto;
    uint16_t external_port;    /* must not be 0 */
    net_ipv4 internal_ip;
    uint16_t internal_port;
    uint32_t lease_seconds;    /* 0 means no expiry, as the IGD spec allows */
    const char *description;
} net_mapping;

/* SSDP M-SEARCH for an IGD. On success writes the control URL (NUL-terminated, at most cap bytes).
 * Stub: returns NET_ERR_UNSUPPORTED. */
int net_upnp_discover(char *control_url, size_t cap);

/* SOAP AddPortMapping / DeletePortMapping / GetExternalIPAddress on WANIPConnection:1 or :2.
 * Argument checks are real (port 0 and a null URL are rejected). Stubs otherwise. */
int net_upnp_add_mapping(const char *control_url, const net_mapping *m);
int net_upnp_delete_mapping(const char *control_url, enum net_proto proto, uint16_t external_port);
int net_upnp_external_ip(const char *control_url, net_ipv4 *out);

#endif
