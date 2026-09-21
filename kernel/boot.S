.section .multiboot2
.align 8
mb2_header_start:
.long 0xe85250d6
.long 0
.long mb2_header_end - mb2_header_start
.long -(0xe85250d6 + 0 + (mb2_header_end - mb2_header_start))
.word 0
.word 0
.long 8
mb2_header_end:

.section .text
.global _start
.type _start, @function
_start:
    cli
    lea stack_top(%rip), %rsp
    xor %rbp, %rbp
    mov %ebx, %edi
    call kernel_main
1:
    hlt
    jmp 1b

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top: