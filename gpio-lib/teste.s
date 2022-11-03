.syntax unified


    .equ    STACK_ARGS,8    @ sp already 8-byte aligned

.section .rodata
.align 2


asc_J:    .ascii "JACA DA BAHIA"



.text
.align 2

stackL0:
    push {r4-r7, fp, lr}
    pop  {r4-r7, fp, lr}
    bx   lr

@ .global main
@ .type   main, %function

@ main:
@     sub     sp, sp, 16      @ space for saving regs
@     str     r4, [sp, 0]     @ save r4
@     str     r5, [sp, 4]     @      r5
@     str     fp, [sp, 8]     @      fp
@     str     lr, [sp, 12]    @      lr
@     add     fp, sp, 12      @ set our frame pointer
@     sub     sp, sp, STACK_ARGS @ sp on 8-byte boundary

@ code:
@     ldr r0, =asc_J
@     add r0, r0, 1
@     ldr r1, [r0, 0]
@     ldr r2, [r0, 1]
@     ldr r3, [r0, 2]
@     ldr r4, [r0, 3]

@     bl stackL0

@ end:
@     mov     r0, 0           @ return 0;
@     add     sp, sp, STACK_ARGS  @ fix sp
@     ldr     r4, [sp, 0]     @ restore r4
@     ldr     r5, [sp, 4]     @      r5
@     ldr     fp, [sp, 8]     @         fp
@     ldr     lr, [sp, 12]    @         lr
@     add     sp, sp, 16      @ restore sp
@     @ bkpt
@     bx      lr              @ return
@     .align  2


.global testa
.type   testa, %function

testa:
    ldr r1, [r0, 0]
    bic r1, 0xffffff00
    add r0, 1
    cmp r1, 0x0
    bne testa
    @ ldr r2, [r0, 1]
    @ bic r2, 0xffffff00
    @ ldr r3, [r0, 2]
    @ bic r3, 0xffffff00
    @ ldr r4, [r0, 3]
    @ bic r4, 0xffffff00

    bx lr

