[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_hlt
	EXTERN	_io_load_eflags
	EXTERN	_io_cli
	EXTERN	_io_out8
	EXTERN	_io_store_eflags
[FILE "bootpack.c"]
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
L2:
	CALL	_io_hlt
	JMP	L2
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
L7:
	MOV	EAX,DWORD [-12+EBP]
	CMP	EAX,DWORD [32+EBP]
	JLE	L10
	JMP	L6
L10:
	MOV	EAX,DWORD [20+EBP]
	MOV	DWORD [-8+EBP],EAX
L11:
	MOV	EAX,DWORD [-8+EBP]
	CMP	EAX,DWORD [28+EBP]
	JLE	L14
	JMP	L9
L14:
	MOV	EAX,DWORD [-12+EBP]
	IMUL	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [-8+EBP]
	MOV	EDX,EAX
	ADD	EDX,DWORD [8+EBP]
	MOV	AL,BYTE [-1+EBP]
	MOV	BYTE [EDX],AL
	LEA	EAX,DWORD [-8+EBP]
	INC	DWORD [EAX]
	JMP	L11
L9:
	LEA	EAX,DWORD [-12+EBP]
	INC	DWORD [EAX]
	JMP	L7
L6:
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
L17:
	MOV	EAX,DWORD [-4+EBP]
	CMP	EAX,DWORD [12+EBP]
	JLE	L20
	JMP	L18
L20:
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
	JMP	L17
L18:
	SUB	ESP,12
	PUSH	DWORD [-8+EBP]
	CALL	_io_store_eflags
	ADD	ESP,16
	LEAVE
	RET