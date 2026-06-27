#include "idt.h"

static struct idt_entry idt[256];
static struct idt_ptr idtp;

static void idt_set_gate(uint8_t vec, void *base, uint16_t sel, uint8_t flags) {
    uint64_t b = (uint64_t)base;
    idt[vec].base_low  = b & 0xFFFF;
    idt[vec].base_mid  = (b >> 16) & 0xFFFF;
    idt[vec].base_high = (b >> 32) & 0xFFFFFFFF;
    idt[vec].selector  = sel;
    idt[vec].ist       = 0;
    idt[vec].flags     = flags;
    idt[vec].zero      = 0;
}

void idt_init(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint64_t)&idt;

    static const void *isrs[] = {&isr_0,&isr_1,&isr_2,&isr_3,&isr_4,&isr_5,&isr_6,&isr_7,
        &isr_8,&isr_9,&isr_10,&isr_11,&isr_12,&isr_13,&isr_14,&isr_15,
        &isr_16,&isr_17,&isr_18,&isr_19,&isr_20,&isr_21,&isr_22,&isr_23,
        &isr_24,&isr_25,&isr_26,&isr_27,&isr_28,&isr_29,&isr_30,&isr_31};
    static const void *irqs[] = {&irq_0,&irq_1,&irq_2,&irq_3,&irq_4,&irq_5,&irq_6,&irq_7,
        &irq_8,&irq_9,&irq_10,&irq_11,&irq_12,&irq_13,&irq_14,&irq_15};

    for (int i = 0; i < 32; i++)
        idt_set_gate(i, (void*)isrs[i], 0x28, 0x8E);
    for (int i = 0; i < 16; i++)
        idt_set_gate(32 + i, (void*)irqs[i], 0x28, 0x8E);
    for (int i = 48; i < 256; i++)
        idt_set_gate(i, (void*)irqs[0], 0x28, 0x8E);

    __asm__ volatile("lidt %0" : : "m"(idtp));
}
