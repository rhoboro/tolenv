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
		GLOBAL _asm_inthandler21, _asm_inthandler2c, _asm_inthandler20
		EXTERN _inthandler21, _inthandler2c, _inthandler20
		GLOBAL _load_cr0, _store_cr0
		GLOBAL _memtest_sub
		GLOBAL _load_tr, _taskswitch4, _taskswitch3

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

;
; PUSH EAX は下記に相当する
; ADD ESP,-4
; MOV [SS:ESP],EAX

; POP EAX は下記に相当する
; MOV EAX,[SS:ESP]
; ADD ESP,4
; ここでやっていることはもとの状態を記録しておき、処理後に戻しているだけ
; C言語はDS, ES, SSが同じセグメントを指定しているという前提で動くので呼び出し前にそのようにしている
_asm_inthandler21:
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV EAX,ESP
        PUSH    EAX
        MOV AX,SS
        MOV DS,AX
        MOV ES,AX
        CALL    _inthandler21
        POP EAX
        POPAD
        POP DS
        POP ES
        IRETD   ; 割り込み終了時にはこれが必要。これはCではできないのでアセンブリでかく

_asm_inthandler2c:
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV EAX,ESP
        PUSH    EAX
        MOV AX,SS
        MOV DS,AX
        MOV ES,AX
        CALL    _inthandler2c
        POP EAX
        POPAD
        POP DS
        POP ES
        IRETD   ; 割り込み終了時にはこれが必要。これはCではできないのでアセンブリでかく


_asm_inthandler20:
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV EAX,ESP
        PUSH    EAX
        MOV AX,SS
        MOV DS,AX
        MOV ES,AX
        CALL    _inthandler20
        POP EAX
        POPAD
        POP DS
        POP ES
        IRETD   ; 割り込み終了時にはこれが必要。これはCではできないのでアセンブリでかく

_load_cr0:      ; int load_cr0(void);
        MOV     EAX,CR0
        RET

_store_cr0:     ; void store_cr0(int);
        MOV     EAX,[ESP+4]
        MOV     CR0,EAX
        RET

_memtest_sub:
        PUSH    EDI                 ; 退避
        PUSH    ESI
        PUSH    EBX
        MOV     ESI,0xaa55aa55      ; pat0 = 0xaa55aa55
        MOV     EDI,0x55aa55aa      ; pat1 = 0x55aa55aa
        MOV     EAX,[ESP+12+4]      ; i = start
mts_loop:
        MOV     EBX,EAX
        ADD     EBX,0xffc           ; p = i + 0xffc
        MOV     EDX,[EBX]           ; old = *p
        MOV     [EBX],ESI
        XOR     DWORD [EBX],0xffffffff  ; *p ^= 0xffffffff
        CMP     EDI,[EBX]           ;  if (*p != pat1) goto fin;
        JNE     mts_fin
        XOR     DWORD [EBX],0xffffffff
        CMP     ESI,[EBX]
        JNE     mts_fin
        MOV     [EBX],EDX
        ADD     EAX,0x1000
        CMP     EAX,[ESP+12+8]
        JNE     mts_loop
        POP     EBX
        POP     ESI
        POP     EDI
        RET
mts_fin:
        MOV     [EBX],EDX           ; *p = old
        POP     EBX
        POP     ESI
        POP     EDI
        RET

_load_tr:       ; void load_tr(int tr);
        LTR     [ESP+4]     ; trレジスタに現在どのタスクを実行しているか保存
        RET

_taskswitch4:
        JMP     4*8:0       ; 4番目のセグメントにタスク　スイッチする
        RET
_taskswitch3:
        JMP     3*8:0       ; 4番目のセグメントにタスク　スイッチする
        RET
