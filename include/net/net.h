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

/* Socket-style addressing. Family and type use the names of their POSIX counterparts. */
enum net_family { NET_AF_INET = 2 };
enum net_type { NET_SOCK_DGRAM = 2, NET_SOCK_STREAM = 1 };

typedef struct net_addr {
    net_ipv4 ip;
    uint16_t port;             /* host byte order */
} net_addr;

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

/* Fixed pools, allocated once at start-up (decision 7). Sizes are a starting point and are
 * revisited with the first real driver. */
#define NET_RX_POOL 64u
#define NET_TX_POOL 64u
#define NET_BUF_SIZE 2048u

#endif
