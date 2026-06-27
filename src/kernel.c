#include "limine.h"
#include <stddef.h>
#include "idt.h"
#include "pic.h"
#include "ps2.h"
#include "compositor.h"

static volatile struct limine_framebuffer_request _framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = NULL,
};

static volatile struct limine_hhdm_request _hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0,
    .response = NULL,
};

__attribute__((used, section(".limine_reqs")))
static volatile void *limine_request_array[] = {
    (void *)&_framebuffer_request,
    (void *)&_hhdm_request,
    NULL
};

static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void serial_puts(const char *str) {
    for (const char *p = str; *p; p++)
        outb(0x3F8, *p);
}

static void serial_hex(uint8_t v) {
    outb(0x3F8, "0123456789ABCDEF"[v >> 4]);
    outb(0x3F8, "0123456789ABCDEF"[v & 0xF]);
}

__attribute__((used)) void isr_dispatcher(regs_t *r) {
    if (r->int_no < 32) {
        outb(0x3F8, '!'); outb(0x3F8, 'E'); serial_hex((uint8_t)r->int_no);
        outb(0x3F8, '\n');
        for (;;) { __asm__("cli; hlt"); }
    }
    uint8_t irq = (uint8_t)(r->int_no - 32);
    if (irq == PS2_KEYB_IRQ) {
        ps2_keyb_isr();
    } else if (irq == PS2_MOUSE_IRQ) {
        ps2_mouse_isr();
    }
    if (irq >= 8)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void _start(void) {
    serial_puts("_start\n");

    struct limine_framebuffer *fb = _framebuffer_request.response->framebuffers[0];
    uint64_t width = fb->width;
    uint64_t height = fb->height;
    uint64_t pitch = fb->pitch;
    void *fb_addr = (void *)fb->address;

    comp_init((uint64_t)fb_addr, width, height, pitch);
    serial_puts("comp_init\n");

    idt_init();
    serial_puts("idt_init\n");

    pic_remap(0x20, 0x28);
    serial_puts("pic_remap\n");

    pic_clear_mask(0);
    pic_clear_mask(1);
    pic_clear_mask(2);
    pic_clear_mask(12);

    ps2_init();
    serial_puts("ps2_init\n");

    __asm__("sti");

    while (1) {
        mouse_packet_t mpkt;
        while (ps2_mouse_get(&mpkt)) {
            int nx = comp.mouse_x + mpkt.dx / 3;
            int ny = comp.mouse_y + mpkt.dy / 3;
            if (nx < 0) nx = 0;
            if (ny < 0) ny = 0;
            if (nx >= (int)comp.width) nx = (int)comp.width - 1;
            if (ny >= (int)comp.height) ny = (int)comp.height - 1;
            comp.mouse_x = nx;
            comp.mouse_y = ny;
            comp.mouse_buttons = mpkt.buttons;
        }

        if ((comp.mouse_buttons & 1) && !(comp.prev_mouse_buttons & 1))
            comp_handle_click();
        comp.prev_mouse_buttons = comp.mouse_buttons;

        int ch = ps2_keyb_getchar();
        if (ch >= 0)
            comp_handle_key(ch);

        comp_render();
        comp_swap();
        __asm__("hlt");
    }
}
