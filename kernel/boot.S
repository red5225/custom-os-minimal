.section .multiboot,"a"
.align 8
mb2_header_start:
    .long 0xE85250D6
    .long 0
    .long mb2_header_end - mb2_header_start
    .long -(0xE85250D6 + 0 + (mb2_header_end - mb2_header_start))
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

.macro serial_mark code
    mov $0x3F8, %dx
    mov $\code, %al
    outb %al, %dx
.endm

.section .bss
.align 4096
pml4: .skip 4096
pdpt: .skip 4096
pd0:  .skip 4096
pd1:  .skip 4096
pd2:  .skip 4096
pd3:  .skip 4096
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
    mov %eax, %ebp
    mov %ebx, %esi
    serial_mark 'A'

    /* Clear PML4, PDPT and all four page directories (24 KiB). */
    cld
    mov $pml4, %edi
    xor %eax, %eax
    mov $6144, %ecx
    rep stosl

    /* PML4 entry 0 MUST point to the PDPT. */
    mov $pdpt, %eax
    or $3, %eax
    mov %eax, pml4

    /* Four PDPT entries map four consecutive 1 GiB ranges. */
    mov $pd0, %eax
    or $3, %eax
    mov %eax, pdpt
    mov $pd1, %eax
    or $3, %eax
    mov %eax, pdpt+8
    mov $pd2, %eax
    or $3, %eax
    mov %eax, pdpt+16
    mov $pd3, %eax
    or $3, %eax
    mov %eax, pdpt+24

    /* Fill the contiguous four page directories with 2 MiB pages. */
    xor %ecx, %ecx
1:
    mov %ecx, %eax
    shl $21, %eax
    or $0x83, %eax
    mov %eax, pd0(,%ecx,8)
    movl $0, pd0+4(,%ecx,8)
    inc %ecx
    cmp $2048, %ecx
    jne 1b
    serial_mark 'B'

    /* Clear EM and TS; enable MP, PAE, OSFXSR and OSXMMEXCPT. */
    mov %cr0, %eax
    and $0xFFFFFFF3, %eax
    or $0x00000002, %eax
    mov %eax, %cr0
    mov %cr4, %eax
    or $0x00000620, %eax
    mov %eax, %cr4

    mov $pml4, %eax
    mov %eax, %cr3
    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    serial_mark 'C'

    lgdt gdt64_pointer
    ljmp $0x08, $long_mode_start

.code64
long_mode_start:
    serial_mark 'D'
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    xor %eax, %eax
    mov %ax, %fs
    mov %ax, %gs
    mov $stack_top, %rsp

    /* System V x86-64 args: RDI=Multiboot2 magic, RSI=info address. */
    mov %ebp, %edi
    mov %esi, %esi
    xor %ebp, %ebp
    serial_mark 'E'
    call kernel_main
2:
    hlt
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
