[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_hlt
	EXTERN	_hankaku
	EXTERN	_io_load_eflags
	EXTERN	_io_cli
	EXTERN	_io_out8
	EXTERN	_io_store_eflags
[FILE "bootpack.c"]
[SECTION .data]
LC0:
	DB	"ABC 123",0x00
LC1:
	DB	"Haribote OS.",0x00
[SECTION .text]
	ALIGN	2
	GLOBAL	_HariMain
_HariMain:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,8
	CALL	_init_palette
	MOV	DWORD [-4+EBP],4080
	SUB	ESP,4
	MOV	EAX,DWORD [-4+EBP]
	MOVSX	EAX,WORD [6+EAX]
	PUSH	EAX
	MOV	EAX,DWORD [-4+EBP]
	MOVSX	EAX,WORD [4+EAX]
	PUSH	EAX
	MOV	EAX,DWORD [-4+EBP]
	PUSH	DWORD [8+EAX]
	CALL	_init_screen
	ADD	ESP,16
	SUB	ESP,8
	PUSH	LC0
	PUSH	7
	PUSH	8
	PUSH	8
	MOV	EAX,DWORD [-4+EBP]
	MOVSX	EAX,WORD [4+EAX]
	PUSH	EAX
	MOV	EAX,DWORD [-4+EBP]
	PUSH	DWORD [8+EAX]
	CALL	_putfont8_asc
	ADD	ESP,32
	SUB	ESP,8
	PUSH	LC1
	PUSH	0
	PUSH	31
	PUSH	31
	MOV	EAX,DWORD [-4+EBP]
	MOVSX	EAX,WORD [4+EAX]
	PUSH	EAX
	MOV	EAX,DWORD [-4+EBP]
	PUSH	DWORD [8+EAX]
	CALL	_putfont8_asc
	ADD	ESP,32
	SUB	ESP,8
	PUSH	LC1
	PUSH	7
	PUSH	30
	PUSH	30
	MOV	EAX,DWORD [-4+EBP]
	MOVSX	EAX,WORD [4+EAX]
	PUSH	EAX
	MOV	EAX,DWORD [-4+EBP]
	PUSH	DWORD [8+EAX]
	CALL	_putfont8_asc
	ADD	ESP,32
L2:
	CALL	_io_hlt
	JMP	L2
	ALIGN	2
	GLOBAL	_putfont8_asc
_putfont8_asc:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,8
	MOV	EAX,DWORD [24+EBP]
	MOV	BYTE [-1+EBP],AL
L6:
	MOV	EAX,DWORD [28+EBP]
	CMP	BYTE [EAX],0
	JNE	L9
	JMP	L5
L9:
	SUB	ESP,8
	MOV	EAX,DWORD [28+EBP]
	MOV	AL,BYTE [EAX]
	AND	EAX,255
	SAL	EAX,4
	ADD	EAX,_hankaku
	PUSH	EAX
	MOVSX	EAX,BYTE [-1+EBP]
	PUSH	EAX
	PUSH	DWORD [20+EBP]
	PUSH	DWORD [16+EBP]
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_putfont8
	ADD	ESP,32
	LEA	EAX,DWORD [16+EBP]
	ADD	DWORD [EAX],8
	LEA	EAX,DWORD [28+EBP]
	INC	DWORD [EAX]
	JMP	L6
L5:
	LEAVE
	RET
	ALIGN	2
	GLOBAL	_putfont8
_putfont8:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,16
	MOV	EAX,DWORD [24+EBP]
	MOV	BYTE [-1+EBP],AL
	MOV	DWORD [-8+EBP],0
L11:
	CMP	DWORD [-8+EBP],15
	JLE	L14
	JMP	L10
L14:
	MOV	EAX,DWORD [-8+EBP]
	ADD	EAX,DWORD [28+EBP]
	MOV	AL,BYTE [EAX]
	MOV	BYTE [-13+EBP],AL
	MOV	EAX,DWORD [-8+EBP]
	ADD	EAX,DWORD [20+EBP]
	IMUL	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [8+EBP]
	ADD	EAX,DWORD [16+EBP]
	MOV	DWORD [-12+EBP],EAX
	MOV	AL,-128
	AND	AL,BYTE [-13+EBP]
	TEST	AL,AL
	JE	L15
	MOV	EDX,DWORD [-12+EBP]
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
L15:
	MOV	AL,BYTE [-13+EBP]
	AND	EAX,64
	TEST	AL,AL
	JE	L16
	MOV	EDX,DWORD [-12+EBP]
	INC	EDX
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
L16:
	MOV	AL,BYTE [-13+EBP]
	AND	EAX,32
	TEST	AL,AL
	JE	L17
	MOV	EDX,DWORD [-12+EBP]
	ADD	EDX,2
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
L17:
	MOV	AL,BYTE [-13+EBP]
	AND	EAX,16
	TEST	AL,AL
	JE	L18
	MOV	EDX,DWORD [-12+EBP]
	ADD	EDX,3
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
L18:
	MOV	AL,BYTE [-13+EBP]
	AND	EAX,8
	TEST	AL,AL
	JE	L19
	MOV	EDX,DWORD [-12+EBP]
	ADD	EDX,4
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
L19:
	MOV	AL,BYTE [-13+EBP]
	AND	EAX,4
	TEST	AL,AL
	JE	L20
	MOV	EDX,DWORD [-12+EBP]
	ADD	EDX,5
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
L20:
	MOV	AL,BYTE [-13+EBP]
	AND	EAX,2
	TEST	AL,AL
	JE	L21
	MOV	EDX,DWORD [-12+EBP]
	ADD	EDX,6
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
L21:
	MOV	AL,BYTE [-13+EBP]
	AND	EAX,1
	TEST	AL,AL
	JE	L13
	MOV	EDX,DWORD [-12+EBP]
	ADD	EDX,7
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
L13:
	LEA	EAX,DWORD [-8+EBP]
	INC	DWORD [EAX]
	JMP	L11
L10:
	LEAVE
	RET
	ALIGN	2
	GLOBAL	_init_screen
_init_screen:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,8
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,29
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	DEC	EAX
	PUSH	EAX
	PUSH	0
	PUSH	0
	PUSH	14
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,28
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	DEC	EAX
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,28
	PUSH	EAX
	PUSH	0
	PUSH	8
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,27
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	DEC	EAX
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,27
	PUSH	EAX
	PUSH	0
	PUSH	7
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	DEC	EAX
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	DEC	EAX
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,26
	PUSH	EAX
	PUSH	0
	PUSH	8
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,24
	PUSH	EAX
	PUSH	59
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,24
	PUSH	EAX
	PUSH	3
	PUSH	7
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,4
	PUSH	EAX
	PUSH	2
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,24
	PUSH	EAX
	PUSH	2
	PUSH	7
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,4
	PUSH	EAX
	PUSH	59
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,4
	PUSH	EAX
	PUSH	3
	PUSH	15
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,5
	PUSH	EAX
	PUSH	59
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,23
	PUSH	EAX
	PUSH	59
	PUSH	15
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,3
	PUSH	EAX
	PUSH	59
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,3
	PUSH	EAX
	PUSH	2
	PUSH	0
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,3
	PUSH	EAX
	PUSH	60
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,24
	PUSH	EAX
	PUSH	60
	PUSH	0
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,24
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	SUB	EAX,4
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,24
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	SUB	EAX,47
	PUSH	EAX
	PUSH	15
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,4
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	SUB	EAX,47
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,23
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	SUB	EAX,47
	PUSH	EAX
	PUSH	15
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,3
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	SUB	EAX,4
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,3
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	SUB	EAX,47
	PUSH	EAX
	PUSH	7
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	SUB	ESP,4
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,3
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	SUB	EAX,3
	PUSH	EAX
	MOV	EAX,DWORD [16+EBP]
	SUB	EAX,24
	PUSH	EAX
	MOV	EAX,DWORD [12+EBP]
	SUB	EAX,3
	PUSH	EAX
	PUSH	7
	PUSH	320
	PUSH	DWORD [8+EBP]
	CALL	_backfill8
	ADD	ESP,32
	LEAVE
	RET
	ALIGN	2
	GLOBAL	_backfill8
_backfill8:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,12
	MOV	EAX,DWORD [16+EBP]
	MOV	BYTE [-1+EBP],AL
	MOV	EAX,DWORD [24+EBP]
	MOV	DWORD [-12+EBP],EAX
L25:
	MOV	EAX,DWORD [-12+EBP]
	CMP	EAX,DWORD [32+EBP]
	JLE	L28
	JMP	L24
L28:
	MOV	EAX,DWORD [20+EBP]
	MOV	DWORD [-8+EBP],EAX
L29:
	MOV	EAX,DWORD [-8+EBP]
	CMP	EAX,DWORD [28+EBP]
	JLE	L32
	JMP	L27
L32:
	MOV	EAX,DWORD [-12+EBP]
	IMUL	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [-8+EBP]
	MOV	EDX,EAX
	ADD	EDX,DWORD [8+EBP]
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
	LEA	EAX,DWORD [-8+EBP]
	INC	DWORD [EAX]
	JMP	L29
L27:
	LEA	EAX,DWORD [-12+EBP]
	INC	DWORD [EAX]
	JMP	L25
L24:
	LEAVE
	RET
[SECTION .data]
_table_rgb.0:
	DB	0
	DB	0
	DB	0
	DB	-1
	DB	0
	DB	0
	DB	0
	DB	-1
	DB	0
	DB	-1
	DB	-1
	DB	0
	DB	0
	DB	0
	DB	-1
	DB	-1
	DB	0
	DB	-1
	DB	0
	DB	-1
	DB	-1
	DB	-1
	DB	-1
	DB	-1
	DB	-58
	DB	-58
	DB	-58
	DB	-124
	DB	0
	DB	0
	DB	0
	DB	-124
	DB	0
	DB	-124
	DB	-124
	DB	0
	DB	0
	DB	0
	DB	-124
	DB	-124
	DB	0
	DB	-124
	DB	0
	DB	-124
	DB	-124
	DB	-124
	DB	-124
	DB	-124
[SECTION .text]
	ALIGN	2
	GLOBAL	_init_palette
_init_palette:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,8
	SUB	ESP,4
	PUSH	_table_rgb.0
	PUSH	15
	PUSH	0
	CALL	_set_palette
	ADD	ESP,16
	LEAVE
	RET
	ALIGN	2
	GLOBAL	_set_palette
_set_palette:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,8
	CALL	_io_load_eflags
	MOV	DWORD [-8+EBP],EAX
	CALL	_io_cli
	SUB	ESP,8
	PUSH	DWORD [8+EBP]
	PUSH	968
	CALL	_io_out8
	ADD	ESP,16
	MOV	EAX,DWORD [8+EBP]
	MOV	DWORD [-4+EBP],EAX
L35:
	MOV	EAX,DWORD [-4+EBP]
	CMP	EAX,DWORD [12+EBP]
	JLE	L38
	JMP	L36
L38:
	SUB	ESP,8
	MOV	EAX,DWORD [16+EBP]
	MOV	AL,BYTE [EAX]
	SHR	AL,2
	AND	EAX,255
	PUSH	EAX
	PUSH	969
	CALL	_io_out8
	ADD	ESP,16
	SUB	ESP,8
	MOV	EAX,DWORD [16+EBP]
	INC	EAX
	MOV	AL,BYTE [EAX]
	SHR	AL,2
	AND	EAX,255
	PUSH	EAX
	PUSH	969
	CALL	_io_out8
	ADD	ESP,16
	SUB	ESP,8
	MOV	EAX,DWORD [16+EBP]
	ADD	EAX,2
	MOV	AL,BYTE [EAX]
	SHR	AL,2
	AND	EAX,255
	PUSH	EAX
	PUSH	969
	CALL	_io_out8
	ADD	ESP,16
	LEA	EAX,DWORD [16+EBP]
	ADD	DWORD [EAX],3
	LEA	EAX,DWORD [-4+EBP]
	INC	DWORD [EAX]
	JMP	L35
L36:
	SUB	ESP,12
	PUSH	DWORD [-8+EBP]
	CALL	_io_store_eflags
	ADD	ESP,16
	LEAVE
	RET
