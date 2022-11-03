.syntax unified

.equ    STMODE, 0x03        
.equ    B4MODE, 0x02        @ Modo de 4 bits (Primeira linha)
.equ    B42LINE,0b00101000  @ Modo de 4 bits (Segunda linha)
.equ    CLEAR,  0b00000001  @ Clears entire display and sets DDRAM address 0 in address counter.
.equ    HOME,   0b00000010  @ Sets DDRAM address 0 in address counter. Also returns display from being shifted to original position. DDRAM contents remain unchanged.

.equ    SHFTL,  0b00000111  @ Shifts the entire display to the left. The cursor follows the display shift.
.equ    SHFTR,  0b00000101  @ Shifts the entire display to the right. The cursor follows the display shift.
.equ    EMSLN,  0b00000110  @ Entry Mode Set

@ Constantes para definir os modos do display
.equ    D1C0B0, 0b00001100  @ Display ON / Cursor OFF / Blinking OFF
.equ    D1C1B0, 0b00001110  @ Display ON / Cursor ON / Blinking OFF
.equ    D1C1B1, 0b00001111  @ Display ON / Cursor ON / Blinking ON

.equ    CSHFTL, 0b00010000
.equ    CSHFTR, 0b00010100
.equ    CPOADR, 0b10000000

.equ    STLNCH, 0x00101000  @ Set 2 lines 5x7 matrix

.equ    CGADDR, 0x01000000  @ Set address MASK

.equ    UPOSCMD, 0b10000001
.equ    DPOSCMD, 0b10000000
