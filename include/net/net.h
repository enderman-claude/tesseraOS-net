/* net: service API for apps, and internal interfaces. Stubs only; see docs/DESIGN.md. */
#ifndef NET_NET_H
#define NET_NET_H

#include <stddef.h>
#include <stdint.h>

enum net_status {
    NET_OK = 0,
    NET_ERR_UNSUPPORTED = -1,  /* stub: not implemented yet */
    NET_ERR_INVALID = -2,      /* bad argument */
    NET_ERR_DENIED = -3,       /* manifest does not allow networking */
    NET_ERR_NODEV = -4,        /* no network device found */
    NET_ERR_NOMEM = -5,        /* packet pool exhausted */
};

/* MAC address: six octets. */
typedef struct net_mac {
    uint8_t octet[6];
} net_mac;

/* IPv4 address in network byte order. */
typedef uint32_t net_ipv4;

/* IPv6 address, 16 octets in network order. */
typedef struct net_ipv6 {
    uint8_t octet[16];
} net_ipv6;

/* Address families use the POSIX values. */
enum net_family { NET_AF_INET = 2, NET_AF_INET6 = 10 };
enum net_type { NET_SOCK_STREAM = 1, NET_SOCK_DGRAM = 2 };

/* An IP address of either family. */
typedef struct net_ip {
    uint8_t family;            /* NET_AF_INET or NET_AF_INET6 */
    union {
        net_ipv4 v4;
        net_ipv6 v6;
    } u;
} net_ip;

/* A socket address: address and port. Ports are host byte order here; the wire encoding is
 * done in the stack. */
typedef struct net_addr {
    net_ip ip;
    uint16_t port;
} net_addr;

/* Port 0 means "pick one" in bind. Ephemeral ports (RFC 6335 dynamic range) are the ones a
 * client socket gets when it connects without binding. */
#define NET_PORT_EPHEMERAL_MIN 49152u
#define NET_PORT_EPHEMERAL_MAX 65535u

static inline net_addr net_addr_v4(net_ipv4 ip, uint16_t port) {
    net_addr a;
    a.ip.family = NET_AF_INET;
    a.ip.u.v4 = ip;
    a.port = port;
    return a;
}

static inline net_addr net_addr_v6(const net_ipv6 *ip, uint16_t port) {
    net_addr a;
    a.ip.family = NET_AF_INET6;
    a.ip.u.v6 = *ip;
    a.port = port;
    return a;
}

/* 1 if the port is in the IANA dynamic range, 0 otherwise. Real code, tested on the host. */
static inline int net_port_is_ephemeral(uint16_t port) {
    /* NET_PORT_EPHEMERAL_MAX is 65535, the largest uint16_t, so only the lower bound is checked. */
    return port >= NET_PORT_EPHEMERAL_MIN;
}

/* How the service gets its IPv4 address. Set in the service manifest.
 * STATIC: use static_ip, netmask and gateway only.
 * DHCP:   ask a DHCP server; static fields are ignored.
 * BOTH:   try DHCP first; if no lease arrives in time, use the static fields. */
enum net_addr_mode {
    NET_ADDR_STATIC = 1,
    NET_ADDR_DHCP = 2,
    NET_ADDR_BOTH = 3,
};

typedef struct net_config {
    enum net_addr_mode mode;
    net_ipv4 static_ip;        /* required for STATIC, and as the fallback for BOTH */
    net_ipv4 netmask;
    net_ipv4 gateway;
} net_config;

/* Fixed pools, allocated once at start-up. Sizes are a starting point. */
#define NET_RX_POOL 64u
#define NET_TX_POOL 64u
#define NET_BUF_SIZE 2048u

#endif
