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

#endif
