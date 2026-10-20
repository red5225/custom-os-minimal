.section .multiboot,"a"
.align 8
mb2_header_start:
    .long 0xe85250d6
    .long 0
    .long mb2_header_end - mb2_header_start
    .long -(0xe85250d6 + (mb2_header_end - mb2_header_start))
    .short 0
    .short 0
    .long 8
mb2_header_end:

.section .bss
.align 4096
pml4: .skip 4096
pdpt: .skip 4096
pdir: .skip 4096
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
    mov $pdir, %edi
    xor %eax, %eax
    mov $3072, %ecx
    rep stosl

    mov $pdir, %eax
    or $0x3, %eax
    mov %eax, pdpt
    mov $pdpt, %eax
    or $0x3, %eax
    mov %eax, pml4

    mov $pdir, %edi
    xor %ecx, %ecx
1:
    mov %ecx, %eax
    shl $21, %eax
    or $0x83, %eax
    mov %eax, (%edi,%ecx,8)
    movl $0, 4(%edi,%ecx,8)
    inc %ecx
    cmp $512, %ecx
    jne 1b

    mov %cr4, %eax
    or $0x20, %eax
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
    lgdt gdt64_pointer
    ljmp $0x08, $long_mode_start

.code64
long_mode_start:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    xor %ax, %ax
    mov %ax, %fs
    mov %ax, %gs
    mov $stack_top, %rsp
    xor %rbp, %rbp
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