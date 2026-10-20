.section .multiboot,"a"
.align 8
mb2_header_start:
    .long 0xe85250d6
    .long 0
    .long mb2_header_end - mb2_header_start
    /* checksum = -(magic + architecture + header length) */
    .long -0xe85250d6 - (mb2_header_end - mb2_header_start)

    /* Request a 32-bit framebuffer; fall back if unavailable. */
    .short 5
    .short 0
    .long 20
    .long 1024
    .long 768
    .long 32
    .align 8
    .short 0
    .short 0
    .long 8
mb2_header_end:

.section .bss
.align 4096
pml4: .skip 4096
pdpt: .skip 4096
pd:   .skip 4096
.align 16
stack_bottom: .skip 16384
stack_top:

.section .text.boot,"ax"
.code32
.global _start
.type _start, @function
_start:
    cli
    mov $stack_top, %esp

    /* Save the Multiboot2 magic and information pointer. */
    mov %eax, %ebp
    mov %ebx, %esi

    /* Clear page tables. */
    cld
    mov $pml4, %edi
    xor %eax, %eax
    mov $3072, %ecx
    rep stosl

    /* Identity-map the first 1 GiB with 2 MiB pages. */
    mov $pdpt, %eax
    or $3, %eax
    mov %eax, pml4
    mov $pd, %eax
    or $3, %eax
    mov %eax, pdpt
    mov $0x00000083, %eax
    mov $0, %ecx
1:
    mov %ecx, %eax
    shl $21, %eax
    or $0x83, %eax
    mov %eax, (pd,%ecx,8)
    movl $0, 4(pd,%ecx,8)
    inc %ecx
    cmp $512, %ecx
    jne 1b

    /* Enable PAE, long mode, and paging. */
    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4
    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr
    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0
    lgdt gdt64_pointer
    ljmp $0x08, $long_mode_start

.code64
long_mode_start:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    xor %eax, %eax
    mov %ax, %fs
    mov %ax, %gs
    mov $stack_top, %rsp

    /* Pass Multiboot2 registers as the C function arguments. */
    mov %ebp, %edi
    mov %esi, %esi
    xor %ebp, %ebp
    call kernel_main
2:  hlt
    jmp 2b

.align 8
gdt64:
    .quad 0
    .quad 0x00AF9A000000FFFF
    .quad 0x00CF92000000FFFF
gdt64_end:
gdt64_pointer:
    .word gdt64_end - gdt64 - 1
    .quad gdt64

.section .note.GNU-stack,"",@progbits
