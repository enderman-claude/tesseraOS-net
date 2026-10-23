/* IP stack interface (inside the net service). Stubs until milestones N2 and N3. */
#ifndef NET_STACK_H
#define NET_STACK_H

#include "net/driver.h"
#include "net/net.h"

typedef struct net_stack net_stack;

/* Set the stack's address and attach a driver. Stub. */
int net_stack_init(net_stack **out, const net_driver *drv, net_ipv4 address);
void net_stack_free(net_stack *s);

/* Answer an ARP request for our address. Stub, milestone N2. */
int net_arp_handle(net_stack *s, const uint8_t *frame, size_t len);

/* Send an ICMP echo request and wait for the reply. Stub, milestone N2. */
int net_icmp_ping(net_stack *s, net_ipv4 dst, uint32_t timeout_ms);

/* UDP send and receive. Stub, milestone N3. */
int net_udp_send(net_stack *s, const net_addr *dst, const uint8_t *data, size_t len);
int net_udp_recv(net_stack *s, uint16_t local_port, uint8_t *buf, size_t cap, net_addr *from);

#endif
