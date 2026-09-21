# ImuxOS

From-scratch x86_64 operating-system project. No Linux kernel and no Linux userspace.

Initial stack:
- C++20 freestanding kernel
- small x86_64 Assembly entry
- CMake
- GRUB Multiboot2 bootstrap
- framebuffer graphics
- QEMU development target
- bootable ISO for USB testing

GRUB is only the initial bootstrap mechanism. It can later be replaced with an ImuxOS-native UEFI boot path.

For Windows development, the target is to build the ISO and launch it with QEMU. Native USB boot uses the same generated boot image.

QEMU example:
qemu-system-x86_64 -cdrom build/ImuxOS.iso -m 512M