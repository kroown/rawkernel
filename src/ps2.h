#ifndef _PS2_H
#define _PS2_H

#include <stdint.h>
#include <stdbool.h>

#define PS2_KEYB_IRQ 1
#define PS2_MOUSE_IRQ 12

void ps2_init(void);

int ps2_keyb_getchar(void);
bool ps2_keyb_empty(void);

typedef struct {
    int dx, dy;
    uint8_t buttons;
} mouse_packet_t;

bool ps2_mouse_get(mouse_packet_t *pkt);
bool ps2_mouse_empty(void);

void ps2_keyb_isr(void);
void ps2_mouse_isr(void);

#endif
