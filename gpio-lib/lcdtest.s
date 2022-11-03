@ Opens the /dev/gpiomem device and maps GPIO memory
@ into program virtual address space.
@ 2017-09-29: Bob Plantz 



@ Define my Raspberry Pi
        .syntax unified         @ modern syntax

        
@ Constant program data
        .section .rodata
        .align  2

        .equ    STACK_ARGS,8    @ sp already 8-byte aligned
        .equ    PBTN1,  0x20        @GPIO05
        .equ    PBTN2,  0x80000     @GPIO19
        .equ    PBTN3,  0x4000000   @GPIO26

@ The program
        .text
        .align  2



        .global main
        .type   main, %function
main:
        sub     sp, sp, 16      @ space for saving regs
        str     r4, [sp, 0]     @ save r4
        str     r5, [sp, 4]     @      r5
        str     fp, [sp, 8]     @      fp
        str     lr, [sp, 12]    @      lr
        add     fp, sp, 12      @ set our frame pointer
        sub     sp, sp, STACK_ARGS @ sp on 8-byte boundary

setup:
        bl      _lcdStartup
        bl      _clearDisplay
        bl      _turnOnCursorOff
        bl      _setMemoryMode


loop:
        mov     r0, #1000
        bl      _mdelay

btn1:
        mov r0, PBTN1
        bl _readIn
        cmp r0, #0
        bgt btn2
        
        ldr r1, =COUNTER
        ldr r0, [r1]
        mov r0, 0x30
        str r0, [r1]

        ldr r1, =COUNTERDEC
        ldr r0, [r1]
        mov r0, 0x30
        str r0, [r1]

btn2:
        mov r0, PBTN2
        bl _readIn
        cmp r0, #0
        bgt btn3
        
        ldr r1, =PAUSE
        ldr r0, [r1]
        cmp r0, #0
        moveq r0, #1
        streq r0, [r1]

btn3:
        mov r0, PBTN3
        bl _readIn
        cmp r0, #0
        bgt timerUpdate
        
        ldr r1, =PAUSE
        ldr r0, [r1]
        cmp r0, #1
        moveq r0, #0
        streq r0, [r1]


timerUpdate:
        bl      _cursorHome

        ldr     r2, =COUNTERDEC
        ldr     r0, [r2]

        bl      _sendChar

        ldr     r2, =COUNTER
        ldr     r0, [r2]

        bl      _sendChar

        ldr r2, =COUNTER
        ldr r0, [r2]
        ldr r4, =PAUSE
        ldr r3, [r4]
        cmp r3, #0
        addeq r0, #1
        

        cmp     r0, 0x39
        movgt   r0, 0x30
        str r0, [r2]
        
        ble loop

        ldr r2, =COUNTERDEC
        ldr r0, [r2]
        ldr r4, =PAUSE
        ldr r3, [r4]
        cmp r3, #0
        add r0, #1
        

        cmp     r0, 0x39
        movgt   r0, 0x30
        str r0, [r2]

  
        b loop


@ fim de programa
        mov     r0, 0           @ return 0;
        add     sp, sp, STACK_ARGS  @ fix sp
        ldr     r4, [sp, 0]     @ restore r4
        ldr     r5, [sp, 4]     @      r5
        ldr     fp, [sp, 8]     @         fp
        ldr     lr, [sp, 12]    @         lr
        add     sp, sp, 16      @ restore sp
        @ bkpt
        bx      lr              @ return
        .align  2




COUNTER_adr:            .word COUNTER

.data
        gpioAdr:        .word 0
        CNTR:           .word 0
        COUNTER:        .word 0x30
        COUNTERDEC:     .word 0x30
        PAUSE:          .byte 0
        POLD:           .word 0, 0, 0 @ PBTN1, PBTN2, PBTN3

