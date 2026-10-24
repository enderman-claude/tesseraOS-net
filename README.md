# tesseraOS-net

Networking for tesseraOS: a virtio-net driver, an IP stack, and a socket-style service over IPC.

Status: **plan with stubs**. Every function reports `NET_ERR_UNSUPPORTED` until its milestone
lands. See [docs/DESIGN.md](docs/DESIGN.md) for the layers, milestones (N0 to N5) and open decisions.

`make test` builds the stubs and checks that each one reports the right status.

## Related repos

- [tesseraOS](https://github.com/enderman-claude/tesseraOS): the OS this service runs on. Master plan: [#1](https://github.com/enderman-claude/tesseraOS/issues/1).
- [tessera](https://github.com/enderman-claude/tessera): the kernel. Needs PCI, DMA, paging and IPC for this service.
- [tesseraOS-teds](https://github.com/enderman-claude/tesseraOS-teds) and [tesseraOS-xteds](https://github.com/enderman-claude/tesseraOS-xteds): other L7 services written the same way (stubs first, then real code).
- [oxinit](https://github.com/enderman-claude/oxinit): its control socket is local (Unix); it will use this service only once sockets exist on tesseraOS.

*Footnote: the network stack is very private. It has no packets to share yet.*

## License

Not yet chosen.
