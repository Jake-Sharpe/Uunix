[bits 64]
global syscall_entry
extern handle_syscall

syscall_entry:
    ; 1. SAVE THE USER CONTEXT
    ; 'syscall' put the return address in RCX and RFLAGS in R11
    push rcx
    push r11

    ; 2. SAVE CALLER-SAVED REGISTERS (ABI Safety)
    push rdi
    push rsi
    push rdx
    push r8
    push r9
    push r10

    ; 3. CALL THE C++ HANDLER
    call handle_syscall

    ; 4. RESTORE CALLER-SAVED REGISTERS
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rsi
    pop rdi

    ; 5. RESTORE THE USER CONTEXT
    pop r11 ; Restore User RFLAGS into R11
    pop rcx ; Restore User RIP into RCX

    ; 6. RETURN TO RING 3
    ; sysretq will:
    ; - Set RIP = RCX
    ; - Set RFLAGS = R11
    ; - Switch CS to User Code and SS to User Data (defined in STAR MSR)
    sysretq