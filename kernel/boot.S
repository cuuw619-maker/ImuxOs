.section .multiboot2
.align 8
mb2_header_start:
.long 0xe85250d6
.long 0
.long mb2_header_end - mb2_header_start
.long -(0xe85250d6 + 0 + (mb2_header_end - mb2_header_start))
.word 5
.word 0
.long 20
.long 1024
.long 768
.long 32
.long 0
.word 0
.word 0
.long 8
mb2_header_end:

.section .text
.global _start
.type _start, @function
_start:
    cli
    mov $0xB8000, %rax
    movw $0x0F49, (%rax)
    movw $0x0F4D, 2(%rax)
    movw $0x0F55, 4(%rax)
    movw $0x0F58, 6(%rax)
    movw $0x0F42, 8(%rax)
    movw $0x0F4F, 10(%rax)
    movw $0x0F4F, 12(%rax)
    movw $0x0F54, 14(%rax)
    movw $0x0F3A, 16(%rax)
    movw $0x0F20, 18(%rax)
    movw $0x0F45, 20(%rax)
    movw $0x0F4E, 22(%rax)
    movw $0x0F54, 24(%rax)
    movw $0x0F52, 26(%rax)
    movw $0x0F59, 28(%rax)

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
