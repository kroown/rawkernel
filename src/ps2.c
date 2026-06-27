#include "ps2.h"
#include <stdbool.h>

#define PS2_DATA    0x60
#define PS2_CMD     0x64
#define PS2_STATUS  0x64

#define KEYBUF_SIZE 64
#define MOUSEBUF_SIZE 32

static char keybuf[KEYBUF_SIZE];
static int keybuf_r, keybuf_w;

static mouse_packet_t mousebuf[MOUSEBUF_SIZE];
static int mousebuf_r, mousebuf_w;

static int mouse_cycle;
static uint8_t mouse_pkt[3];
static inline uint8_t inb(uint16_t port) {
    uint8_t v;
    __asm__ volatile("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

static int ps2_send_cmd(uint8_t cmd) {
    for (int i = 0; i < 1000; i++) {
        if (!(inb(PS2_STATUS) & 2)) {
            outb(PS2_CMD, cmd);
            return 0;
        }
        io_wait();
    }
    return -1;
}

static int ps2_send_data(uint8_t data) {
    for (int i = 0; i < 1000; i++) {
        if (!(inb(PS2_STATUS) & 2)) {
            outb(PS2_DATA, data);
            return 0;
        }
        io_wait();
    }
    return -1;
}

static int ps2_recv_data(uint8_t *out) {
    for (int i = 0; i < 1000; i++) {
        if (inb(PS2_STATUS) & 1) {
            *out = inb(PS2_DATA);
            return 0;
        }
        io_wait();
    }
    return -1;
}

static uint8_t keyb_us[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0
};

static void kbd_event(uint8_t scancode) {
    bool pressed = !(scancode & 0x80);
    uint8_t code = scancode & 0x7F;
    if (code >= 128) return;
    if (!pressed) return;
    uint8_t c = keyb_us[code];
    if (c == 0) return;
    int next = (keybuf_w + 1) % KEYBUF_SIZE;
    if (next != keybuf_r) {
        keybuf[keybuf_w] = c;
        keybuf_w = next;
    }
}

static void mouse_event(void) {
    int next = (mousebuf_w + 1) % MOUSEBUF_SIZE;
    if (next != mousebuf_r) {
        mouse_packet_t pkt;
        pkt.dx = (int)mouse_pkt[1];
        if (mouse_pkt[0] & 0x10) pkt.dx -= 256;
        pkt.dy = (int)mouse_pkt[2];
        if (mouse_pkt[0] & 0x20) pkt.dy -= 256;
        pkt.dy = -pkt.dy;
        pkt.buttons = mouse_pkt[0] & 7;
        mousebuf[mousebuf_w] = pkt;
        mousebuf_w = next;
    }
}

__attribute__((used)) void ps2_keyb_isr(void) {
    uint8_t status = inb(PS2_STATUS);
    if (!(status & 1)) return;
    uint8_t data = inb(PS2_DATA);
    kbd_event(data);
}

__attribute__((used)) void ps2_mouse_isr(void) {
    uint8_t status = inb(PS2_STATUS);
    if (!(status & 1)) return;
    uint8_t data = inb(PS2_DATA);
    mouse_pkt[mouse_cycle] = data;
    mouse_cycle++;
    if (mouse_cycle == 3) {
        mouse_cycle = 0;
        if (mouse_pkt[0] & 0x08)
            mouse_event();
    }
}

int ps2_keyb_getchar(void) {
    if (keybuf_r == keybuf_w) return -1;
    char c = keybuf[keybuf_r];
    keybuf_r = (keybuf_r + 1) % KEYBUF_SIZE;
    return c;
}

bool ps2_keyb_empty(void) {
    return keybuf_r == keybuf_w;
}

bool ps2_mouse_get(mouse_packet_t *pkt) {
    if (mousebuf_r == mousebuf_w) return false;
    *pkt = mousebuf[mousebuf_r];
    mousebuf_r = (mousebuf_r + 1) % MOUSEBUF_SIZE;
    return true;
}

bool ps2_mouse_empty(void) {
    return mousebuf_r == mousebuf_w;
}

static void ps2_flush(void) {
    while (inb(PS2_STATUS) & 1)
        inb(PS2_DATA);
}

void ps2_init(void) {
    ps2_flush();
    uint8_t cfg = 0;

    ps2_send_cmd(0x20);
    ps2_recv_data(&cfg);

    cfg |= 3;
    ps2_send_cmd(0x60);
    ps2_send_data(cfg);

    ps2_send_cmd(0xAE);
    ps2_send_cmd(0xA8);

    ps2_send_cmd(0x20);
    ps2_recv_data(&cfg);
    cfg |= 1;
    cfg &= ~0x30;
    ps2_send_cmd(0x60);
    ps2_send_data(cfg);

    ps2_send_data(0xF4);
    uint8_t ack;
    ps2_recv_data(&ack);

    ps2_send_cmd(0xD4);
    ps2_send_data(0xF4);
    ps2_recv_data(&ack);

    if (ps2_send_cmd(0xD4) == 0 && ps2_send_data(0xE8) == 0 && ps2_send_cmd(0xD4) == 0 && ps2_send_data(3) == 0) {
        ps2_send_cmd(0xD4);
        ps2_send_data(0xF3);
        ps2_recv_data(&ack);
        ps2_send_cmd(0xD4);
        ps2_send_data(100);
        ps2_recv_data(&ack);
    }

    ps2_flush();
}
