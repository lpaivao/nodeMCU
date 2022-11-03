@ Opens the /dev/gpiomem device and maps GPIO memory
@ into program virtual address space.
@ 2017-09-29: Bob Plantz


@ Define my Raspberry Pi
        @ .cpu    arm1176jz-s
        @ .cpu    cortex-a7
        .syntax unified         @ modern syntax

@ Constants for assembler
@ The following are defined in /usr/include/asm-generic/fcntl.h:
@ Note that the values are specified in octal.
        .equ    O_RDWR,00000002   @ open for read/write
        .equ    O_DSYNC,00010000  @ synchronize virtual memory
        .equ    __O_SYNC,04000000 @      programming changes with
        .equ    O_SYNC,__O_SYNC|O_DSYNC @ I/O memory
@ The following are defined in /usr/include/asm-generic/mman-common.h:
        .equ    PROT_READ,0x1   @ page can be read
        .equ    PROT_WRITE,0x2  @ page can be written
        .equ    MAP_SHARED,0x01 @ share changes

@ The following are defined by me:
.if .pimodel == 2
        .equ    PERIPH,0x3f000000   @ RPi 2 & 3 peripherals
.endif
.if .pimodel == 0
        .equ    PERIPH,0x20000000   @ RPi zero & 1 peripherals
.endif
        .equ    O_FLAGS,O_RDWR|O_SYNC @ open file flags
        .equ    PROT_RDWR,PROT_READ|PROT_WRITE
        .equ    NO_PREF,0
        .equ    PAGE_SIZE,4096  @ Raspbian memory page
        .equ    FILE_DESCRP_ARG,0   @ file descriptor
        .equ    DEVICE_ARG,4        @ device address
        .equ    STACK_ARGS,8    @ sp already 8-byte aligned

@ Endereços e offsets para oum GPIO
        .equ    GPIO_OFFSET,0x200000    @ start of GPIO device
        .equ    GPFSEL0,0x0             @ Offset do reg GPFSEL0
        .equ    GPFSEL1,0x4             
        .equ    GPFSEL2,0x8             
        .equ    GPFSEL3,0xC             
        .equ    GPFSEL4,0x10             
        .equ    GPFSEL5,0x14             
        .equ    GPCLR0,0x28             @
        .equ    GPCLR1,0x2C             @
        .equ    GPSET0,0x1c             @
        .equ    GPSET1,0x20             @
        .equ    GPLEV0,0x34             @

        .equ    FSEL_OUTPUT,0b001       @ Bits de modo de OUTPUT
        .equ    FSEL_MASK,  0b111

@ Endereços e offsets para o TIMER
        .equ    TIMER_OFFSET,0x3000  @ start of GPIO device
        .equ    TCLO,0x4
        .equ    TCHI,0x8
        .equ    BASEUSEC, 1000

@ Constantes para mapeamento de pinos do LCD
        .equ	BIT0, 0b1
        .equ	BIT1, 0b10
        .equ	BIT2, 0b100
        .equ	BIT3, 0b1000
        @bitN -> GPIOA [OFF=A-N]
        .equ    D4O, 12 @ GPIO12
        .equ    D5O, 15 @ GPIO16
        .equ    D6O, 18 @ GPIO20
        .equ    D7O, 18 @ GPIO21


@ Constant program data
        .section .rodata
        .align  2
device:
        @ .asciz  "/dev/gpiomem"
        .asciz  "/dev/mem"

        .text
        .align  2

.macro _openfile filedesc_adr
        ldr     r0, deviceAddr  @ address of /dev/gpiomem
        ldr     r1, openMode    @ flags for accessing device
        bl      open
        ldr     r4, \filedesc_adr
        str     r0, [r4]
.endm


.macro _memmap base_adr, filedesc_reg, mappedadr_adr
@ Map the address registers to a virtual memory location so we c[an access them        
        push    {fp, lr}
        sub     sp, sp, #8

        str     \filedesc_reg, [sp, FILE_DESCRP_ARG] @ /dev/mem file descriptor
        ldr     r0, \base_adr        @ address of GPIO
        str     r0, [sp, DEVICE_ARG]      @ location of GPIO
        mov     r0, NO_PREF     @ let kernel pick memory
        mov     r1, PAGE_SIZE   @ get 1 page of memory
        mov     r2, PROT_RDWR   @ read/write this memory
        mov     r3, MAP_SHARED  @ share with other processes
        bl      mmap
        ldr     r1, \mappedadr_adr
        str     r0, [r1]

        add     sp, sp, #8
        pop     {fp, lr}
.endm

.macro _setreg port_adr, regmask, data, data_pos
        ldr r0, \port_adr
        ldr r0, [r0] 
        mov r1, \regmask
        bic r0, r0, r1, lsl \data_pos
        mov r1, \data
        lsl r1, r1, \data_pos
        orr r0, r0, r1
        ldr r1, \port_adr
        str r0, [r1]
.endm


.macro _bitset mask, pos
        mov r10, \mask		@ mascara do bit
	and r10, r11, r10	@ isolamento do bit da mascara
	lsl r10, r10, \pos	@ posicionamento do resultado no pino correspondente
	orr r9, r9, r10
.endm

.macro _mapbitsToPort4 val, port
	ldr r12, \port 	@ endereço da porta
	mov r11, \val		@ valor a ser mapeado
	ldr r9, [r12]		@ racupera o valor da porta
        _bitset BIT0, D4O
        _bitset BIT1, D5O
        _bitset BIT2, D6O
        _bitset BIT3, D7O
	str r9, [r12]		@ atualiza a porta
.endm

.macro mdelay msec, timer_adr
        ldr r10, \timer_adr
        ldr r10, [r10]          @ endereço do timer
        mov r9, BASEUSEC
        mul r9, r9, \msec
        ldr r11, [r10, TCLO]    @ momento inicial
\@:     
        ldr r12, [r10, TCLO]
        sub r12, r12, r11
        cmp r12, r9
        blt \@b
.endm

.macro udelay msec, timer_adr
        ldr r10, \timer_adr
        ldr r10, [r10]          @ endereço do timer
        mov r9, \msec
        ldr r11, [r10, TCLO]    @ momento inicial
\@:     
        ldr r12, [r10, TCLO]
        sub r12, r12, r11
        cmp r12, r9
        blt \@b
.endm


deviceAddr:
        .word   device
openMode:
        .word   O_FLAGS

gpio:
        .word   PERIPH+GPIO_OFFSET
timer:
        .word   PERIPH+TIMER_OFFSET


