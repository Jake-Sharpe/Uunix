ENTRY(_start)

SECTIONS {
    . = 1M;

    .boot : {
        /* Align multiboot header as early as possible */
        KEEP(*(.boot))
    }

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }
    _kernel_end = .;
}
