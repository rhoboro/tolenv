; haribote-os
; TAB=4

; BOOT_INFO
CYLS		EQU	0x0ff0	; ブートセクタで設定
LEDS		EQU	0x0ff1
VMODE		EQU	0x0ff3	; 何ビットカラーか
SCRNX		EQU	0x0ff4	; 解像度X
SCRNY		EQU	0x0ff6	; 解像度Y
VRAM		EQU	0x0ff8	; グラフィックバッファの開始位置

		ORG	0xc200	; このプログラムがどこに読み込まれるか

		MOV	AL,0x13	;VGAグラフィックス, 320x200x8bitカラー
		MOV	AH,0x00
		INT	0x10
		MOV	BYTE [VMODE],8	; メモ
		MOV	WORD [SCRNX],320	; メモ
		MOV	WORD [SCRNY],200	; メモ
		MOV	DWORD [VRAM],0x000a0000	; VRAMの内容はディスプレイの画素と対応している。値は画面モードごとに違う

; キーボードのLED状態をBIOSから取得

		MOV	AH,0x02
		INT	0x16
		MOV	[LEDS],AL

fin:
		HLT
		JMP	fin
