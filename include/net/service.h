/* Net service (L7): the app-facing socket API. Stubs until milestones N3 to N5.
 *
 * Decision 6: only the net service owns the NIC. Apps never get a raw device capability;
 * they reach the network through the socket calls below, over an IPC endpoint. */
#ifndef NET_SERVICE_H
#define NET_SERVICE_H

#include "net/net.h"

typedef struct net_service net_service;

/* Start the service with an address configuration. Returns NET_ERR_INVALID for a bad config
 * (unknown mode, or a missing static address where one is required). Otherwise a stub returns
 * NET_ERR_UNSUPPORTED. */
int net_service_start(net_service **out, const net_config *cfg);
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
