@ Opens the /dev/gpiomem device and maps GPIO memory
@ into program virtual address space.
@ 2017-09-29: Bob Plantz 


.include "funcs.s"
.include "lcd_instructions.s"

@ Define my Raspberry Pi
        @ .cpu    arm1176jz-s
        .cpu    cortex-a7
        .syntax unified         @ modern syntax

        .equ    FSEL08, 24
        .equ    FSEL12, 6
        .equ    FSEL16, 18
        .equ    FSEL20, 0
        .equ    FSEL21, 3
        .equ    FSEL25, 15

        .equ    OUTMODE, 0b001
        .equ    REGMASK, 0b111

        .equ    GPIO12, 12
        .equ    GPIO16, 16
        .equ    GPIO20, 20
        .equ    GPIO21, 21
        .equ    RS,     25
        .equ    EN,     8

        .equ    ENPIN,  0x100
        .equ    RSPIN,  0x2000000

        .equ    INTERVALO, 5000
        .equ    PULSEINT, 10

        .equ    CLEANMASK, 0x2311100

        .equ    GPLEV0, 0x34


@ Constant program data
        .section .rodata
        .align  2


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

openfile:       @ Abertura do arquivo de espelahamento de memoria /dev/mem
        _openfile fileDescriptor_adr

memorymap:      @ Mapeamento de memória para GPIO
        _memmap gpio, r0, gpioAddress_adr
        ldr r0, fileDescriptor_adr
        ldr r0, [r0]
        _memmap timer, r0, tmAddress_adr
        
readin:
        ldr r0, gpioAddress_adr
        ldr r0, [r0]

        ldr r1, [r0, GPLEV0]    @ carrega os valores no registrador de nivel dos GPIO
        mvn r2, 0b100000        @ Mascara da posição do GPIO5 ( inversa )
        bic r1, r1, r2          @ Limpa os valores dos outros pinos restando somente o GPIO5




        
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


fileDescriptor_adr:     .word fileDescriptor
gpioAddress_adr:        .word gpioAddress
tmAddress_adr:          .word tmAddress
GPORT_adr:              .word GPORT             @ Armazena os bits antes de enviar



.data
        fileDescriptor: .word 0
        gpioAddress:    .word 0
        tmAddress:      .word 0
        GPORT:          .word 0


