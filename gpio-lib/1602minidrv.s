.include "funcs.s"
.include "lcd_instructions.s"

.syntax unified

@Constantes

.section .rodata
.align 2

@Offsets de registradores

    @Seleção de funcção de pinos
    .equ    FSEL01, 3   @EN (GPIO01)
    .equ    FSEL12, 6   @D4 (GPIO12)
    .equ    FSEL16, 18  @D5 (GPIO16)
    .equ    FSEL20, 0   @D6 (GPIO20)
    .equ    FSEL21, 3   @D7 (GPIO21)
    .equ    FSEL25, 15  @RS (GPIO25)

    @Nomeação dos pinos
    .equ    GPIO12, 12
    .equ    GPIO16, 16
    .equ    GPIO20, 20
    .equ    GPIO21, 21
    .equ    RS,     25
    .equ    EN,     1   

    @Constante de pinos
    .equ    ENPIN,  0x2
    .equ    RSPIN,  0x2000000
    .equ    PBTN1,  0x20        @GPIO05
    .equ    PBTN2,  0x80000     @GPIO19
    .equ    PBTN3,  0x4000000   @GPIO26

@Constantes de funções

    @ Pulso de enable
    .equ    PULSEINT, 10

    @ Período de contagem do timer
    .equ    INTERVALO, 1000

    @ Mascara de limpeza de estados de pinos
    .equ    CLEANMASK, 0x2311120

    .equ    OUTMODE, 0b001
    .equ    REGMASK, 0b111



.text
.align 2


@ Zera o estado do(s) pino(s) selecionado(s) em pinmsk
.macro pinClr pinmsk
        mov r11, \pinmsk
        ldr r12, gpioAddress_adr
        ldr r12, [r12]
        str r11, [r12, GPCLR0]
.endm 

@ Seta o estado do(s) pino(s) selecionado(s) em pinmsk
.macro pinSet pinmsk
        mov r11, \pinmsk
        ldr r12, gpioAddress_adr
        ldr r12, [r12]
        str r11, [r12, GPSET0]
.endm 



@ Gera um pulso no pino EN (ENable) para confirmar um comando ou dado
.global _pulseEnable
.type   _pulseEnable, %function

_pulseEnable:
        pinClr ENPIN                        @zera pino enable
        udelay PULSEINT, tmAddress_adr      @espera um pequeno intervalo
        pinSet ENPIN                        @eleva o nível do pino enable
        udelay PULSEINT, tmAddress_adr      @espera um pequeno intervalo
        pinClr ENPIN                        @zera pino enable

        bx  lr


@ Funções internas

@ Escreve um nibble para a memória e para a porta configurada
_write4bits:
         _mapbitsToPort4 r0, GPORT_adr
        ldr r12, gpioAddress_adr
        ldr r12, [r12]
        ldr r11, GPORT_adr
        ldr r11, [r11]
        str r11, [r12, GPSET0]

        bx lr


@ Limpa um nibble da memória e da porta configurada
_clean4bits:
        _mapbitsToPort4 r0, GPORT_adr
        ldr r12, gpioAddress_adr
        ldr r12, [r12]
        ldr r11, GPORT_adr
        ldr r11, [r11]
        str r11, [r12, GPCLR0]
        ldr r11, GPORT_adr
        mov r10, #0
        str r10, [r11]

        bx lr


@ Envia um comando de configuração do modo do LCD (Para inicialização apenas)
@ r0 --> MODO a ser enviado

_setmode:
        @ mov r7, lr      @ salvando o ponteiro para a saída 
        push {fp, lr}

        bl _write4bits

        mov r6, #10
        udelay r6, tmAddress_adr

        bl _pulseEnable

        mov r6, #10
        udelay r6, tmAddress_adr

        bl _clean4bits

        mov r6, #5
        mdelay r6, tmAddress_adr

        pop {fp, lr}
        bx lr



@ Executa os passos para enviar um comando para o LCD
.macro sendCmd CMD
        @ Primeiro os bits altos
        mov r0, \CMD
        lsr r0, #4
        bl _write4bits

        bl _pulseEnable


        mov r0, \CMD
        lsr r0, #4
        bl _clean4bits
        

        @ Segundo os bits baixos
        mov r0, \CMD
        bl _write4bits

        bl _pulseEnable

        mov r0, \CMD
        bl _clean4bits
.endm

@ Executa os passos para enviar um comando para o LCD
.macro sendCmdM CMD
        @ Primeiro os bits altos
        push {r0} 

        lsr r0, #4
        bl _write4bits

        bl _pulseEnable


        ldr r0, [sp, 0]
        lsr r0, #4
        bl _clean4bits
        

        @ Segundo os bits baixos
        ldr r0, [sp, 0]
        bl _write4bits

        bl _pulseEnable

        ldr r0, [sp, 0]
        bl _clean4bits

        pop {r0}
.endm

.macro sendData CMD

        pinSet RSPIN

        @ Primeiro os bits altos
        mov r0, \CMD
        lsr r0, #4
        bl _write4bits

        bl _pulseEnable

        mov r0, \CMD
        lsr r0, #4
        bl _clean4bits
             
        @ Segundo os bits baixos
        mov r0, \CMD
        bl _write4bits

        bl _pulseEnable

        mov r0, \CMD
        bl _clean4bits

        pinClr RSPIN
.endm

.macro sendDataM

        pinSet RSPIN

        @ ldr r1, TMPCHAR_adr
        @ str r0, [r1]
        push {r0}
        
        @ Primeiro os bits altos
        lsr r0, #4
        bl _write4bits

        bl _pulseEnable

        @ ldr r1, TMPCHAR_adr
        @ ldr r0, [r1]
        ldr r0, [sp, 0]
        lsr r0, #4
        bl _clean4bits
             
        @ Segundo os bits baixos
        @ ldr r1, TMPCHAR_adr
        @ ldr r0, [r1]
        ldr r0, [sp, 0]
        bl _write4bits

        bl _pulseEnable

        @ ldr r1, TMPCHAR_adr
        @ ldr r0, [r1]
        ldr r0, [sp, 0]
        bl _clean4bits
        pop {r0}
        pinClr RSPIN
.endm


.global _lcdStartup
.type   _lcdStartup, %function

_lcdStartup:
@ salva o ponto de retorno na Stack
        push {fp, lr}


@ Abertura do arquivo de espelahamento de memoria /dev/mem
        _openfile fileDescriptor_adr
@ Mapeamento de memória para GPIO
        _memmap gpio, r0, gpioAddress_adr
        ldr r0, fileDescriptor_adr
        ldr r0, [r0]
        _memmap timer, r0, tmAddress_adr
        
@ configuração de modo dos GPIOs
        @ Gpios do banco GPFSEL2
        _setreg GPORT_adr, REGMASK, OUTMODE, FSEL20
        _setreg GPORT_adr, REGMASK, OUTMODE, FSEL21
        _setreg GPORT_adr, REGMASK, OUTMODE, FSEL25

        ldr r0, gpioAddress_adr
        ldr r0, [r0]                    @ Endereço base dos perif
        ldr r1, GPORT_adr
        ldr r1, [r1]                    @ Valor configurado da porta
        str r1, [r0, GPFSEL2]           @ Salva no registrador
        ldr r1, GPORT_adr
        mov r0, #0
        str r0, [r1]                    @ Zera a porta

        @ Gpios do banco GPFSEL0
        _setreg GPORT_adr, REGMASK, OUTMODE, FSEL01

        ldr r0, gpioAddress_adr
        ldr r0, [r0]                    @ Endereço base dos perif
        ldr r1, GPORT_adr
        ldr r1, [r1]                    @ Valor configurado da porta
        str r1, [r0, GPFSEL0]                    @ Salva no registrador
        ldr r1, GPORT_adr
        mov r0, #0
        str r0, [r1]                    @ Zera a porta

        @ Gpios do banco GPFSEL1
        _setreg GPORT_adr, REGMASK, OUTMODE, FSEL12
        _setreg GPORT_adr, REGMASK, OUTMODE, FSEL16
        
        ldr r0, gpioAddress_adr
        ldr r0, [r0]
        mov r6, r0
        ldr r1, GPORT_adr
        ldr r1, [r1]                    @ Valor configurado da porta
        str r1, [r0, GPFSEL1]                    @ Salva no registrador

        ldr r0, GPORT_adr
        mov r1, 0
        str r1, [r0]

        ldr r0, =CLEANMASK
        str r0, [r6, GPCLR0]

@ Configura o modo correto para comunicação 4 bits
        mov r1, STMODE
        bl _setmode
        mov r1, STMODE
        bl _setmode
        mov r1, STMODE
        bl _setmode

        mov r1, B4MODE
        bl _setmode

@ Configura o modo de linhas para 2 Linhas
        sendCmd B42LINE
        mov r6, #5
        mdelay r6, tmAddress_adr

        @ recupera link de retorno

        pop {fp, lr}
        bx  lr

@ Fim _lcdStartup


.global _clearDisplay
.type   _clearDisplay, %function

_clearDisplay:
@ salva o ponto de retorno na Stack
        push {fp, lr}

        sendCmd CLEAR

        mov r6, #500
        mdelay r6, tmAddress_adr
        
                @ recupera link de retorno
        pop {fp, lr}
        bx  lr
@ Fim _clearDisplay


.global _turnOnCursorOff
.type   _turnOnCursorOff, %function

_turnOnCursorOff:
@ salva o ponto de retorno na Stack
        push {fp, lr}

        sendCmd D1C0B0

        mov r6, #5
        mdelay r6, tmAddress_adr

                @ recupera link de retorno
        pop {fp, lr}
        bx  lr
@ Fim _turnOnCursorOff

.global _setMemoryMode
.type   _setMemoryMode, %function

_setMemoryMode:
@ salva o ponto de retorno na Stack
        push {fp, lr}

        sendCmd EMSLN

        mov r6, #5
        mdelay r6, tmAddress_adr
        
                @ recupera link de retorno
        pop {fp, lr}
        bx  lr
@ Fim _setMemoryMode


.global _cursorHome
.type   _cursorHome, %function

_cursorHome:
        @ salva o ponto de retorno na Stack
        push {fp, lr}

        sendCmd DPOSCMD

        mov r6, #50
        udelay r6, tmAddress_adr
        
        @ recupera link de retorno
        pop {fp, lr}
        bx  lr
@ Fim _cursorHome

.global _sendChar
.type   _sendChar, %function

_sendChar:
        @ salva o ponto de retorno na Stack
        push    {fp, lr}

        sendDataM

        mov r6, #50
        udelay r6, tmAddress_adr
        
        @ recupera link de retorno
        pop     {fp, lr}
        bx      lr
@ Fim _sendChar

.global _printStr
.type   _printStr, %function

_printStr:
        push {fp, lr}

        mov r1, r0      @ Copia o endereço da string para r1
        
        pinSet RSPIN
        ldr r0, [r1]    @ Carrega a string em r0
loop:   
        bic r0, 0xffffff00 @ Isolamento do caractere
        sendCmdM        @ Envia o caractere em r0
        add r1, 1       @ Itera a string para o proximo char
        ldr r0, [r1]    @ Carrega a string em r0

        mov r6, #50
        udelay r6, tmAddress_adr

        cmp r0, 0x0     @ Avalia o fim da string (char 0x0)
        bne loop        @ Se o char no registrador não for o fim, volta o loop

        pinClr RSPIN

        mov r6, #50
        udelay r6, tmAddress_adr

        pop     {fp, lr}
        bx      lr
@ Fim _printStr

.global _udelay
.type   _udelay, %function
_udelay:
        push    {fp, lr}
        mov     r6, r0
        udelay  r6, tmAddress_adr
        pop     {fp, lr}
        bx      lr
@Fim _udelay

.global _mdelay
.type	_mdelay, %function
_mdelay:
        push	{fp, lr}
        mov     r6, r0
        mdelay  r6, tmAddress_adr
        pop	{fp, lr}
        bx	lr
@ Fim _mdelay

.global _readIn
.type   _readIn, %function
_readIn:
        push {fp, lr}

        ldr r4, gpioAddress_adr
        ldr r4, [r4]

        ldr r1, [r4, GPLEV0]    @ carrega os valores no registrador de nivel dos GPIO
        mvn r2, r0              @ Mascara da posição do GPIO5 ( inversa )
        bic r1, r1, r2          @ Limpa os valores dos outros pinos restando somente o GPIO5
        mov r0, r1

        pop  {fp, lr}
        bx  lr
@ Fim _readIn

.global _getGpioAdr
.type   _getGpioAdr, %function
_getGpioAdr: @ r0 destVarAdr
        push    {fp, lr}

        ldr r1, gpioAddress_adr
        ldr r1, [r1]
        str r1, [r0]

        pop     {fp, lr}
        bx      lr


fileDescriptor_adr:     .word fileDescriptor
gpioAddress_adr:        .word gpioAddress
tmAddress_adr:          .word tmAddress
GPORT_adr:              .word GPORT             @ Armazena os bits antes de enviar
TMPCHAR_adr:            .word TMPCHAR

.data
        fileDescriptor: .word 0
        gpioAddress:    .word 0
        tmAddress:      .word 0
        GPORT:          .word 0
        TMPCHAR:        .word 0
