/* Net service (L7): the app-facing socket API. Stubs until milestones N3 to N5. */
#ifndef NET_SERVICE_H
#define NET_SERVICE_H

#include "net/net.h"

typedef struct net_service net_service;

/* Start the service with the given driver and static address. Stub. */
int net_service_start(net_service **out, net_ipv4 address);
void net_service_stop(net_service *s);

/* Socket-style calls. Each returns a socket handle (>= 0) or a negative net_status.
 * Stubs: all return NET_ERR_UNSUPPORTED. */
int net_socket(net_service *s, int family, int type);
int net_bind(net_service *s, int sock, const net_addr *local);
int net_connect(net_service *s, int sock, const net_addr *remote);
int64_t net_send(net_service *s, int sock, const uint8_t *data, size_t len);
int64_t net_recv(net_service *s, int sock, uint8_t *buf, size_t cap);
int net_close(net_service *s, int sock);

#endif
