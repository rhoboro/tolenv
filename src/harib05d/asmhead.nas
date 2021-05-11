; haribote-os boot asm
; TAB=4

BOTPAK	EQU		0x00280000		; bootpackのロード先
DSKCAC	EQU		0x00100000		; ディスクキャッシュの場所
DSKCAC0	EQU		0x00008000		; ディスクキャッシュの場所（リアルモード）

; BOOT_INFO関係
CYLS	EQU		0x0ff0			; ブートセクタが設定する
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 色数に関する情報。何ビットカラーか？
SCRNX	EQU		0x0ff4			; 解像度のX
SCRNY	EQU		0x0ff6			; 解像度のY
VRAM	EQU		0x0ff8			; グラフィックバッファの開始番地

		ORG		0xc200			; このプログラムがどこに読み込まれるのか

; 画面モードを設定

		MOV		AL,0x13			; VGAグラフィックス、320x200x8bitカラー
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 画面モードをメモする（C言語が参照する）
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; キーボードのLED状態をBIOSに教えてもらう

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; PICが一切の割り込みを受け付けないようにする
;	AT互換機の仕様では、PICの初期化をするなら、
;	こいつをCLI前にやっておかないと、たまにハングアップする
;	PICの初期化はあとでやる

		MOV		AL,0xff
		OUT		0x21,AL         ; マスタの割り込みを禁止。ALはアキュムレーターの下位8bit
		NOP						; OUT命令を連続させるとうまくいかない機種があるらしいので
		OUT		0xa1,AL         ; スレーブの割り込みを禁止

		CLI						; さらにCPUレベルでも割り込み禁止

; CPUから1MB以上のメモリにアクセスできるように、A20GATEを設定

		CALL	waitkbdout      ; wait_KBC_sendready と同じ
		MOV		AL,0xd1
		OUT		0x64,AL         ; ポート0x64に1バイトを送る。0x64はこれからコマンド送るよ。
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20。
		OUT		0x60,AL         ; A20GATE信号線をONにしてね。ONになるとメモリすべてにアクセスできるようになる。互換性のためデフォルトは最大1MBになっている。
		CALL	waitkbdout

; プロテクトモード移行

[INSTRSET "i486p"]				; 486の命令まで使いたいという記述

		LGDT	[GDTR0]			; 暫定GDTを設定
		MOV		EAX,CR0         ; ここから4行でCR0レジスタを更新。「32ビットプロテクトモードを使いたい」という意味になる。セグメント野解釈にGDTが使われるようになる = 32ビットモード
		AND		EAX,0x7fffffff	; bit31を0にする（ページング禁止のため）
		OR		EAX,0x00000001	; bit0を1にする（プロテクトモード移行のため）。通常のアプリがセグメント情報を書き換えられなくする
		MOV		CR0,EAX         ; 正確にはプロテクテッドバーチャルアドレスモード。GDT（セグメント=仮想アドレス）を使わせることで物理アドレスに直接アクセスできなくする
		JMP		pipelineflush   ; モード変更で命令の意味が変わるのでJMPしてリフレッシュする

; モード変更に合わせてセグメントレジスタの値を変更する。0x0008はGDT+1のセグメント。
pipelineflush:
		MOV		AX,1*8			;  読み書き可能セグメント32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpackの転送

		MOV		ESI,bootpack	; コピーしたい関数を指定
		MOV		EDI,BOTPAK		; コピー先を指定
		MOV		ECX,512*1024/4  ; コピーするサイズを指定。サイズはダブルワード(4バイト)単位
		CALL	memcpy          ; 実行。下の2つも同様。

; ついでにディスクデータも本来の位置へ転送
; 下記2つでメモリの 0x00100000 以降にブートセクタを含むディスクデータがすべて乗る

; まずはブートセクタから

		MOV		ESI,0x7c00		; 転送元
		MOV		EDI,DSKCAC		; 転送先
		MOV		ECX,512/4
		CALL	memcpy

; 残り全部。諸々の計算はブートセクタの直後に載せるため。

		MOV		ESI,DSKCAC0+512	; 転送元
		MOV		EDI,DSKCAC+512	; 転送先
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; シリンダ数からバイト数/4に変換
		SUB		ECX,512/4		; IPLの分だけ差し引く
		CALL	memcpy

; asmheadでしなければいけないことは全部し終わったので、
;	あとはbootpackに任せる

; bootpackの起動

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4; >>=2 の意味で右に2回シフト(Shift Right)。
		JZ		skip			; 転送するべきものがない。Jump if zero で直前の結果によってジャンプするか決まる。
		MOV		ESI,[EBX+20]	; 転送元
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 転送先
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; スタック初期値
		JMP		DWORD 2*8:0x0000001b    ; CSに 2*8 を代入して 0x1b に飛ぶ特別なJUMP。2番目のセグメントにある0x1b( 物理的には0x28001b になる)はbootpack.hrbのある場所。

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; ANDの結果が0でなければwaitkbdoutへ
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 引き算した結果が0でなければmemcpyへ
		RET
; memcpyはアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でも書ける

		ALIGNB	16
GDT0:
		RESB	8				; ヌルセレクタ
		DW		0xffff,0x0000,0x9200,0x00cf	; 読み書き可能セグメント32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 実行可能セグメント32bit（bootpack用）

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
