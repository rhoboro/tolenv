; naskfunc
; TAB=4

[FORMAT "WCOFF"]	; オブジェクトファイル作成モード
[INSTRSET "i486p"]	; 486の命令まで使う。これがないとECXなど32ビット用レジスタはラベル扱いされる
[BITS 32]	; 32ビットモードの機械語を作成

; オブジェクトファイルのための情報

[FILE "naskfunc.nas"]
	GLOBAL _io_hlt,_write_mem8	; このプログラムに含まれる関数名

[SECTION .text]

_io_hlt:	; void io_hlt(void);
	HLT
        RET

_write_mem8:	; void write_mem8(int addr, int data);
	MOV	ECX,[ESP+4]	; addr をロードEAX, ECX, EDXのみCと連携できる
	MOV	AL,[ESP+8]	; data をロード
	MOV	[ECX],AL	; data を書き込む
	RET
