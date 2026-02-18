# vnd for NEXTSTEP

This is a port of the BSD vnd driver to NeXT. It allows mounting disk
images by having a device backed by a file.

## Current status

It loads, you can create a device and select a file, but mounting the
device will panic (in vndstrategy -> vndstart, `dma_list: zero_pfnum`).

## Build/run

Very sketchy for now. You may need to edit `load.sh` and select some
random NeXT format floppy or disk.

```sh
cd KernelServer
./load.sh
mkdir /mnt
sync # you may regret not doing this
mount /dev2/vnd0a /mnt
```

## Notes

The vnd (formerly vn, formerly fd) driver was written by Mike Hibler
at the University of Utah, partly out of the HP300 port of 4.3BSD. The
CSRG merged support for that in time for 4.3BSD-Reno.

The version this is from is from Darwin 0.3, which is a based on a
~1995 version when NeXT merged 4.4/NetBSD code into NS4. This tries to
port the driver back to NeXT's bastard 4.3BSD+Mach port, which is more
like pre-Reno 4.3BSD. Maybe it'd be a better idea to port the ancient
Reno version of the driver instead.
