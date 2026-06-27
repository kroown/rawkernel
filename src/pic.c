#include "pic.h"

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

static uint16_t pic_mask = 0xFFFF;

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile("inb %1, %0" : "=a"(val) : "d"(port));
    return val;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static void io_wait(void) {
    outb(0x80, 0);
}

void pic_remap(int offset_master, int offset_slave) {
    uint8_t m = inb(PIC1_DATA);
    uint8_t s = inb(PIC2_DATA);
    pic_mask = (uint16_t)s << 8 | m;

    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset_master);
    io_wait();
    outb(PIC2_DATA, offset_slave);
    io_wait();
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, m);
    io_wait();
    outb(PIC2_DATA, s);
    io_wait();
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC2_CMD, 0x20);
    outb(PIC1_CMD, 0x20);
}

void pic_set_mask(uint8_t irq) {
    pic_mask |= (1 << irq);
    outb(PIC1_DATA, pic_mask & 0xFF);
    outb(PIC2_DATA, (pic_mask >> 8) & 0xFF);
}

void pic_clear_mask(uint8_t irq) {
    pic_mask &= ~(1 << irq);
    outb(PIC1_DATA, pic_mask & 0xFF);
    outb(PIC2_DATA, (pic_mask >> 8) & 0xFF);
}
