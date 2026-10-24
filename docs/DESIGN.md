# net: networking for tesseraOS

Status: **plan with stubs**. Nothing here sends or receives a packet yet. Every stub returns
`NET_ERR_UNSUPPORTED`, so callers can be written and tested before the real code exists. Decisions
are recorded below; the config check in `net_service_start` is real code.

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
| N6 | IPv6: neighbour discovery, SLAAC, ICMPv6 echo, UDP and TCP over IPv6 | `ping6` from the QEMU host gets a reply |
| N7 | UPnP port mapping: SSDP discovery, SOAP AddPortMapping and DeletePortMapping on the QEMU router | A mapping added by tesseraOS shows in the router's list |

## Dependencies on tessera

- PCI enumeration (bus scan, BAR reads). Part of driver_classes D1 and L6. Not started.
- MSI or polling for receive. Polling is acceptable for N1.
- DMA buffers: physical memory from the page allocator, and an IOMMU if the board has one. QEMU
  without an IOMMU is fine for N1 to N4.
- Paging, so buffers have known physical addresses (M2 in the master plan).
- IPC endpoints (L2) for the service interface.

## Decisions

Answers from the project owner, recorded here. Each is implemented in the stubs as noted.

1. **Stack: leverage open source.** The IP stack is **lwIP 2.2.1** (BSD-3-Clause), pinned as a
   submodule at `third_party/lwip`. Our code is the NIC driver, the glue to the service, and the
   IPC interface. Integration starts at N2. Nothing is compiled from it yet.
2. **Standards.** Confirmed: IPv4, IPv6, correct ports and UPnP port mapping over SOAP (see
   Requirements). **Open:** the question asked how far the first real milestone goes; this answer
   does not settle that. The first milestone stays at N2 until confirmed.
3. **Socket model.** Sockets-style calls over IPC (`include/net/service.h`).
4. **Hardware: generic first.** The generic target is virtio-net over modern virtio 1.x. Specific
   hardware has stubs only, so the interface covers it, but no driver is written
   (`src/driver/wifi_stub.c`). Wi-Fi is therefore "supported" at the interface level and not in
   practice.
5. **Addressing: static, DHCP, or both.** `enum net_addr_mode` has `STATIC`, `DHCP` and `BOTH`.
   `BOTH` tries DHCP first and falls back to the static address. The mode comes from the manifest.
   Config checks run at start-up (`net_service_start`).
6. **Device ownership (my call).** Only the net service owns the NIC. Apps reach the network only
   through socket calls over IPC. A raw device capability would let one app read another app's
   traffic, and sockets cover every use the stack needs.
7. **Packet buffers (my call).** A fixed pool allocated at start-up: 64 receive and 64 transmit
   buffers of 2048 bytes (`include/net/net.h`). Fixed pools make memory use predictable and avoid
   allocation in the receive path. The sizes are a starting point, to be revisited with the first
   real driver.

## Requirements (modern standards)

Confirmed by the project owner: IPv4, IPv6, correct port handling, and router port mapping over
SOAP. The reference for each part:

| Area | Standard | Notes |
|---|---|---|
| IPv4 | RFC 791, host rules from RFC 1122 | Required |
| IPv6 | RFC 8200 | Required. Neighbour discovery RFC 4861, SLAAC RFC 4862, ICMPv6 RFC 4443 |
| DHCPv4 | RFC 2131 | Needed for DHCP and BOTH modes |
| DHCPv6 | RFC 8415 | IPv6 addressing from a server. Later milestone |
| TCP | RFC 9293 (obsoletes RFC 793) | Both families |
| UDP | RFC 768 | Both families |
| Ports | IANA registry, RFC 6335. Ephemeral 49152 to 65535 | `net_port_is_ephemeral`; bind with port 0 picks one |
| Router mapping | UPnP Device Architecture 2.0: SSDP discovery (UDP 239.255.255.250:1900), then SOAP over HTTP on the IGD `WANIPConnection:1` or `:2` service | AddPortMapping, DeletePortMapping, GetExternalIPAddress |
| Alternatives | NAT-PMP RFC 6886, PCP RFC 6887 | Only if a router has no UPnP. Not planned |

The SOAP calls are the usual IGD actions: each is an XML envelope POSTed to the control URL found
during SSDP discovery, with the `SOAPAction` header naming the service and the action.

## Tests

- Host (`make test`): stubs return `NET_ERR_UNSUPPORTED` and nothing else. Each stub is listed in a
  table, so adding a real implementation means updating the table on purpose.
- QEMU (from N1): the commands in the milestone table, run by `make qemu-net-test` once it exists.
