; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; オブジェクトファイルを作るモード	
[INSTRSET "i486p"]				; 486の命令まで使いたいという記述
[BITS 32]						; 32ビットモード用の機械語を作らせる
[FILE "naskfunc.nas"]			; ソースファイル名情報

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL  _load_gdtr, _load_idtr

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS という意味
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS という意味
		RET

; GDTRはセグメント情報を管理する48ビット(=6バイト)の特別なレジスタ
; LGDT命令は指定したアドレスから6バイトをGDTRに読み込む
; レジスタの下位2バイト（メモリの最初の2バイト）がリミットでGDTの有効バイト数-1になる
; 残りの4バイトでGDTの置いてあるアドレスを表す
; DWORD [ESP+4] にリミット(0x0000ffff)が、DWORD [ESP+8] にGDTのアドレス(0x00270000)が入っている
; リトルエンディアンなので FF FF 00 00 00 00 27 00 (左端がESP+4)
; 最終的にGDTRに入れたいのは FF FF 00 00 27 00
; AXに[ESP+4] (=FF FF 00 00) を代入し、
; [ESP+6](00 00 00 00) に AX を代入すると FF FF FF FF 00 00 27 00 になる
;
_load_gdtr:
        MOV     AX,[ESP+4]  ; limit
        MOV     [ESP+6],AX
        LGDT    [ESP+6]
        RET

_load_idtr:
        MOV     AX,[ESP+4]  ; limit
        MOV     [ESP+6],AX
        LIDT    [ESP+6]
        RET
