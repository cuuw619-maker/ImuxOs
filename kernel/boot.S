.section .multiboot2
.align 8
mb2_header_start:
.long 0xe85250d6
.long 0
.long mb2_header_end - mb2_header_start
.long -(0xe85250d6 + 0 + (mb2_header_end - mb2_header_start))

# Request a 1024x768x32 framebuffer from GRUB.
.word 5
.word 0
.long 20
.long 1024
.long 768
.long 32

# Multiboot2 end tag.
.word 0
.word 0
.long 8
mb2_header_end:

.section .text
.global _start
.type _start, @function

# GRUB enters a Multiboot2 kernel in 32-bit protected mode.
# We explicitly enter long mode before executing the x86_64 C++ kernel.
.code32
_start:
    cli
    movl %ebx, %esi
    movl $stack_top, %esp
    xorl %ebp, %ebp

    lgdt gdt_descriptor
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    # Identity-map the first 4 GiB with 2 MiB pages.
    movl $pml4_table, %eax
    movl %eax, %cr3

    movl %cr4, %eax
    orl $0x20, %eax
    movl %eax, %cr4

    # Enable EFER.LME.
    movl $0xC0000080, %ecx
    rdmsr
    orl $0x100, %eax
    wrmsr

    # Enable paging. With PAE + LME this enters long mode.
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    ljmp $0x18, $long_mode_entry

.code64
long_mode_entry:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs

    # Multiboot2 info pointer was preserved from EBX.
    movl %esi, %edi
    call kernel_main

1:
    hlt
    jmp 1b

.section .data
.align 4096
pml4_table:
    .quad pdpt_table + 0x003
    .fill 511, 8, 0

.align 4096
pdpt_table:
    .quad pd_table + 0x003
    .quad pd_table + 0x1003
    .quad pd_table + 0x2003
    .quad pd_table + 0x3003
    .fill 508, 8, 0

.align 4096
pd_table:
    .set page, 0
    .rept 2048
    .quad page + 0x083
    .set page, page + 0x200000
    .endr

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .rodata
.align 8
gdt:
    # null
    .quad 0x0000000000000000
    # 32-bit code, selector 0x08
    .quad 0x00cf9a000000ffff
    # data, selector 0x10
    .quad 0x00cf92000000ffff
    # 64-bit code, selector 0x18
    .quad 0x00af9a000000ffff

gdt_descriptor:
    .word gdt_descriptor - gdt - 1
    .long gdt
