OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

PHDRS
{
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2));
    rodata  PT_LOAD    FLAGS((1 << 2));
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2));
}

SECTIONS
{
    . = 0xffffffff80000000 + SIZEOF_HEADERS;
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .text : {
        *(.text .text.*)
        . = ALIGN(CONSTANT(MAXPAGESIZE));
    } :text

    .rodata : ALIGN(CONSTANT(MAXPAGESIZE)) {
        *(.rodata .rodata.*)
        . = ALIGN(CONSTANT(MAXPAGESIZE));
    } :rodata

    .limine_reqs : ALIGN(CONSTANT(MAXPAGESIZE)) {
        KEEP(*(.limine_reqs))
        . = ALIGN(CONSTANT(MAXPAGESIZE));
    } :data

    .data : ALIGN(CONSTANT(MAXPAGESIZE)) {
        *(.data .data.*)
        . = ALIGN(CONSTANT(MAXPAGESIZE));
    } :data

    .bss : ALIGN(CONSTANT(MAXPAGESIZE)) {
        *(COMMON)
        *(.bss .bss.*)
        . = ALIGN(CONSTANT(MAXPAGESIZE));
    } :data

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
        *(.comment)
    }
}
