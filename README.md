# rawkernel

x86-64 kernel · bios · c

a graphical desktop gui operating system for x86-64 bios, booting via limine.

## features

- composited window manager with double buffering
- ps/2 keyboard and mouse drivers
- start menu with app launcher
- terminal app with built-in commands
- click-to-focus window management
- window dragging, minimise, close

## usage

```bash
qemu-system-x86_64 -m 256M -drive file=build/limine.img,format=raw -serial stdio -vga vmware -display gtk -no-reboot -no-shutdown
```

## build

requirements: x86_64-elf-gcc, limine, mkfs.fat, mtools (mcopy), qemu-img

```bash
git clone https://github.com/kroown/rawkernel.git
cd rawkernel
./build.sh build
```

## architecture

```
src/
  kernel.c        — entry point, main loop, isr dispatcher
  compositor.c    — window manager, compositor, terminal, start menu
  ps2.c           — ps/2 controller, keyboard and mouse drivers
  pic.c           — 8259 pic remap and masking
  idt.c           — interrupt descriptor table setup
  isr_stubs.S     — isr/irq entry stubs
  linker.ld       — kernel layout (higher-half, limine reqs)
```
