; naskfunc
; TAB=4

[FORMAT "WCOFF"]	; オブジェクトファイル作成モード
[BITS 32]	; 32ビットモードの機械語を作成

; オブジェクトファイルのための情報

[FILE "naskfunc.nas"]
	GLOBAL _io_hlt	; このプログラムに含まれる関数名

[SECTION .text]

_io_hlt:	; void io_hlt(void);
	HLT
        RET
