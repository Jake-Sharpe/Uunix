[bits 32]
section .boot

; --- Multiboot2 Header ---
align 8
header_start:
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    align 8
    framebuffer_tag_start:
        dw 5
        dw 0
        dd framebuffer_tag_end - framebuffer_tag_start
        dd 1280
        dd 720
        dd 32
    framebuffer_tag_end:

    align 8
    dw 0
    dw 0
    dd 8
header_end:

global _start
extern kmain

_start:
    mov esi, ebx
    mov esp, stack_top

    ; 1. Paging setup
    mov edi, page_table_l4
    mov ecx, (4096 * 6) / 4 ; Clear L4, L3, and 4x L2
    xor eax, eax
    rep stosd

    ; PML4[0] -> PDP
    mov eax, page_table_l3
    or eax, 0b11
    mov [page_table_l4], eax

    ; PDP[0..3] -> 4x PDs (covers 4GB)
    mov eax, page_table_l2
    or eax, 0b11
    mov [page_table_l3 + 0], eax
    
    add eax, 4096
    mov [page_table_l3 + 8], eax
    
    add eax, 4096
    mov [page_table_l3 + 16], eax
    
    add eax, 4096
    mov [page_table_l3 + 24], eax

    ; PDs[0..2047] -> 2MB Huge Pages (identity maps 4GB)
    mov ecx, 0
.loop_pd:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011 ; Present, Writable, Huge
    mov [page_table_l2 + ecx * 8], eax
    inc ecx
    cmp ecx, 2048
    jne .loop_pd

    ; 2. Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 3. Enable Long Mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 4. Enable Paging
    mov eax, page_table_l4
    mov cr3, eax
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; 5. GDT
    lgdt [gdt64_ptr]
    jmp 0x08:long_mode_start

[bits 64]
long_mode_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov rdi, rsi
    call kmain

    cli
.halt:
    hlt
    jmp .halt

section .bss
align 4096
page_table_l4: resb 4096
page_table_l3: resb 4096
page_table_l2: resb 4096 * 4
stack_bottom:  resb 4096 * 16
stack_top:

section .rodata
align 8
gdt64:
    dq 0 ; null
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; kernel code 0x08
    dq (1<<41) | (1<<44) | (1<<47)           ; kernel data 0x10
    dq (1<<41) | (1<<44) | (1<<47) | (3<<45) ; user data 0x18
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) | (3<<45) ; user code 0x20
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64
