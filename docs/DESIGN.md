# net: networking for tesseraOS

Status: **plan with stubs**. Nothing here sends or receives a packet yet. Every stub returns
`NET_ERR_UNSUPPORTED`, so callers can be written and tested before the real code exists.

Master plan: enderman-claude/tesseraOS#1. Related: tessera `docs/spec/layers.md` (network is an
L7 service that reaches hardware only through IPC), `docs/spec/driver_classes.md` (a network card
is a D1 direct device), and tesseraOS-teds / tesseraOS-xteds (the same stub-first pattern).

## Layers

```
apps  --(IPC: sockets-style messages)-->  net service (L7)
                                           |  IP stack (ARP, IPv4, ICMP, UDP, TCP)
                                           |  frames
                                          NIC driver (D1, virtio-net)  --(BAR, MSI, DMA)-->  device
```

1. **NIC driver** (`src/driver`, D1). Owns one virtio-net device: BAR and MSI from the kernel, DMA
   through an IOMMU domain once tessera has one. Interface: send a frame, receive a frame, read the
   MAC address. Stub: `net_driver_*` return `NET_ERR_UNSUPPORTED`.
2. **IP stack** (`src/stack`, inside the net service). ARP, IPv4, ICMP echo first; UDP, then TCP.
   Stub: the functions exist and return `NET_ERR_UNSUPPORTED`.
3. **Service** (`src/service`, L7). Owns the driver and the stack. Apps reach it through an IPC
   endpoint. It checks each request against the app's manifest. Stub: the socket-style calls return
   `NET_ERR_UNSUPPORTED`.

## API for apps (`include/net/net.h`)

A sockets-style surface, so a libc port (oxinit, musl) can map `socket`, `bind`, `connect`,
`send`, `recv`, and `close` onto it later. Names are tessera-specific until the syscall ABI is decided.

## Milestones

| Step | Work | Done when |
|---|---|---|
| N0 | Stubs, interfaces, host tests (this repo) | `make test` passes |
| N1 | virtio-net driver under QEMU: find the device, read the MAC, send and receive raw frames | A frame sent from tessera is seen by a QEMU host capture (`-object filter-dump`) |
| N2 | ARP and ICMP echo: answer pings | `ping` from the QEMU host gets a reply |
| N3 | UDP sockets: send and receive | A UDP echo test in QEMU round-trips a datagram |
| N4 | TCP: connect, send, receive, close | An HTTP GET to a host web server in QEMU returns its page |
| N5 | Sockets API from apps, through the service | An Ember program (once the tessera ABI exists) makes a UDP round trip |

## Dependencies on tessera

- PCI enumeration (bus scan, BAR reads). Part of driver_classes D1 and L6. Not started.
- MSI or polling for receive. Polling is acceptable for N1.
- DMA buffers: physical memory from the page allocator, and an IOMMU if the board has one. QEMU
  without an IOMMU is fine for N1 to N4.
- Paging, so buffers have known physical addresses (M2 in the master plan).
- IPC endpoints (L2) for the service interface.

## Decisions (open)

1. **TCP/IP stack.** Write our own, or port an open-source stack? Proposed: port **lwIP** (BSD
   licence, built for small embedded targets). It would be a submodule under `third_party/`, like zstd
   in tessera. Our code would be the driver and the glue. Not decided.
2. **Scope of the first real milestone.** Proposed: stop at N2 (ARP and ICMP) before any transport.
3. **Socket model.** Sockets-style calls over IPC (proposed), or a raw frame API only?
4. **Virtio transport.** Legacy virtio-pci (simpler, what QEMU defaults to with `-device virtio-net-pci`)
   or modern virtio 1.0 with capabilities? Proposed: modern, since QEMU supports it.
5. **Addressing.** Static IP from the manifest first, or DHCP at N2? Proposed: static first.
6. **Who owns the device.** Only the net service, or can apps hold a raw NIC capability?
   Proposed: only the net service.
7. **Packet buffers.** Fixed pool allocated at start-up (proposed), or dynamic?

## Tests

- Host (`make test`): stubs return `NET_ERR_UNSUPPORTED` and nothing else. Each stub is listed in a
  table, so adding a real implementation means updating the table on purpose.
- QEMU (from N1): the commands in the milestone table, run by `make qemu-net-test` once it exists.
