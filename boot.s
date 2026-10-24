/* Multiboot v1 entry point for a 32-bit x86 kernel. */
.set MB_MAGIC,    0x1BADB002
.set MB_FLAGS,    0x00000003
.set MB_CHECKSUM, -(MB_MAGIC + MB_FLAGS)

.section .multiboot,"a"
.align 4
.long MB_MAGIC
.long MB_FLAGS
.long MB_CHECKSUM

.section .text.boot
.global start
.type start, @function
.extern kmain
.extern __bss_start
.extern __bss_end

start:
    cli                         /* Do not accept interrupts before setup. */
    cld                         /* String instructions move forward. */

    /* Clear BSS ourselves; the stack is reserved inside BSS. */
    mov $__bss_start, %edi
    mov $__bss_end, %ecx
    sub %edi, %ecx
    xor %eax, %eax
    shr $2, %ecx
    rep stosl

    /* Establish a 16-byte-aligned stack before entering C. */
    mov $stack_top, %esp
    and $-16, %esp
    xor %ebp, %ebp
    call kmain

.hang:
    cli
    hlt
    jmp .hang
.size start, . - start

.section .bss
.align 16
stack_bottom:
.skip 16384                     /* 16 KiB bootstrap stack. */
stack_top:

.section .note.GNU-stack,"",@progbits
