; -----------------------------------------------------------------------------
; Freestanding test for custom kernel
; Syscall 1: Print String (RDI = pointer to string)
; -----------------------------------------------------------------------------

bits 64                         ; Ensure 64-bit mode

section .data
    hello_msg db "Hello world!", 0 ; Null-terminated string

section .text
    global _start

_start:
    ; --- Setup the syscall ---
    mov rax, 1                  ; Custom Syscall ID 1
    mov rdi, hello_msg          ; RDI = Pointer to the string
    
    syscall                     ; Trigger the kernel's syscall handler

    ; --- Optional: Exit or Hang ---
    ; If your kernel has an exit syscall (e.g., syscall 60), call it here.
    ; Otherwise, we loop forever so the CPU doesn't execute garbage.
.hang:
    jmp .hang
