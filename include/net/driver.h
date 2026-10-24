/* NIC driver interface (D1). One driver per device; the net service owns it.
 *
 * Decision: target generic, standard hardware first. virtio-net (modern virtio 1.x) is the
 * generic device. Specific hardware (for example Wi-Fi chipsets) has stubs only: the
 * interface exists so the stack can be wired up, but no driver is written for it. */
#ifndef NET_DRIVER_H
#define NET_DRIVER_H

#include "net/net.h"

#define NET_MAX_FRAME 1518u

typedef struct net_driver {
    const char *name;
    /* Find and initialise the device. Returns NET_OK or an error. */
    int (*open)(void *ctx);
    /* Copy the device MAC address into *mac. */
    int (*mac)(void *ctx, net_mac *mac);
    /* Queue one Ethernet frame for transmit. len must be at most NET_MAX_FRAME. */
    int (*send)(void *ctx, const uint8_t *frame, size_t len);
    /* Copy one received frame into buf. Returns its length, 0 if none is waiting, or an error (< 0). */
    int (*recv)(void *ctx, uint8_t *buf, size_t cap);
    void *ctx;
} net_driver;

/* Generic: virtio-net over modern virtio 1.x. Stub until milestone N1. */
const net_driver *net_driver_virtio(void);

/* Specific hardware: Wi-Fi. Stub only; every call reports NET_ERR_UNSUPPORTED and no
 * chipset is supported. Present so the interface is fixed before any real driver. */
const net_driver *net_driver_wifi_stub(void);

#endif
