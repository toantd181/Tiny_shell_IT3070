.section .data
msg:
    .asciz "Hello, World!\n"

.section .text
.globl _start

_start:
    mov     $1, %rax        # syscall number (sys_write)
    mov     $1, %rdi        # file descriptor (stdout)
    lea     msg(%rip), %rsi # message to write
    mov     $14, %rdx       # message length
    syscall

    mov     $60, %rax       # syscall number (sys_exit)
    xor     %rdi, %rdi      # exit code 0
    syscall
