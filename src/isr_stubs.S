.intel_syntax noprefix
.code64

.macro ISR_NOERR num
.global isr_\num
isr_\num:
    push 0
    push \num
    jmp isr_common
.endm

.macro ISR_ERR num
.global isr_\num
isr_\num:
    push \num
    jmp isr_common
.endm

.macro IRQ num
.global irq_\num
irq_\num:
    push 0
    push \num + 32
    jmp irq_common
.endm

ISR_NOERR 0;  ISR_NOERR 1;  ISR_NOERR 2;  ISR_NOERR 3
ISR_NOERR 4;  ISR_NOERR 5;  ISR_NOERR 6;  ISR_NOERR 7
ISR_ERR   8;  ISR_NOERR 9;  ISR_ERR   10; ISR_ERR   11
ISR_ERR   12; ISR_ERR   13; ISR_ERR   14; ISR_NOERR 15
ISR_NOERR 16; ISR_NOERR 17; ISR_NOERR 18; ISR_NOERR 19
ISR_NOERR 20; ISR_NOERR 21; ISR_NOERR 22; ISR_NOERR 23
ISR_NOERR 24; ISR_NOERR 25; ISR_NOERR 26; ISR_NOERR 27
ISR_NOERR 28; ISR_NOERR 29; ISR_NOERR 30; ISR_NOERR 31

IRQ 0;  IRQ 1;  IRQ 2;  IRQ 3
IRQ 4;  IRQ 5;  IRQ 6;  IRQ 7
IRQ 8;  IRQ 9;  IRQ 10; IRQ 11
IRQ 12; IRQ 13; IRQ 14; IRQ 15

.extern isr_dispatcher

isr_common:
    push rax; push rbx; push rcx; push rdx
    push rsi; push rdi; push rbp
    push r8;  push r9;  push r10; push r11
    push r12; push r13; push r14; push r15

    mov rdi, rsp
    cld
    call isr_dispatcher

    pop r15; pop r14; pop r13; pop r12
    pop r11; pop r10; pop r9;  pop r8
    pop rbp; pop rdi; pop rsi
    pop rdx; pop rcx; pop rbx; pop rax
    add rsp, 16
    iretq

irq_common:
    push rax; push rbx; push rcx; push rdx
    push rsi; push rdi; push rbp
    push r8;  push r9;  push r10; push r11
    push r12; push r13; push r14; push r15

    mov rdi, rsp
    cld
    call isr_dispatcher

    pop r15; pop r14; pop r13; pop r12
    pop r11; pop r10; pop r9;  pop r8
    pop rbp; pop rdi; pop rsi
    pop rdx; pop rcx; pop rbx; pop rax
    add rsp, 16
    iretq

.section .note.GNU-stack,"",@progbits
