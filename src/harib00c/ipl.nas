; haribote-ipl
; TAB=4

		ORG		0x7c00			; このプログラムがどこに読み込まれるのか

; 以下は標準的なFAT12フォーマットフロッピーディスクのための記述

		JMP		entry
		DB		0x90
		DB		"HARIBOTE"		; ブートセクタの名前を自由に書いてよい（8バイト）
		DW		512				; 1セクタの大きさ（512にしなければいけない）
		DB		1				; クラスタの大きさ（1セクタにしなければいけない）
		DW		1				; FATがどこから始まるか（普通は1セクタ目からにする）
		DB		2				; FATの個数（2にしなければいけない）
		DW		224				; ルートディレクトリ領域の大きさ（普通は224エントリにする）
		DW		2880			; このドライブの大きさ（2880セクタにしなければいけない）
		DB		0xf0			; メディアのタイプ（0xf0にしなければいけない）
		DW		9				; FAT領域の長さ（9セクタにしなければいけない）
		DW		18				; 1トラックにいくつのセクタがあるか（18にしなければいけない）
		DW		2				; ヘッドの数（2にしなければいけない）
		DD		0				; パーティションを使ってないのでここは必ず0
		DD		2880			; このドライブ大きさをもう一度書く
		DB		0,0,0x29		; よくわからないけどこの値にしておくといいらしい
		DD		0xffffffff		; たぶんボリュームシリアル番号
		DB		"HARIBOTEOS "	; ディスクの名前（11バイト）
		DB		"FAT12   "		; フォーマットの名前（8バイト）
		TIMES	18 DB 0				; とりあえず18バイトあけておく

; プログラム本体

entry:
		MOV		AX,0			; レジスタ初期化
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; ディスクを読む
; 最初のセクタがブートセクタなので2番目のセクタを指定している
; ループで18番目のセクタまで読み込む
; つまり、 512x17=8704バイトをメモリ0x8200~0xa3ffに読み込む

		MOV		AX,0x8200	; ESx16+BX で読み込むメモリアドレスを表現している
		MOV		ES,AX
		MOV		CH,0	; シリンダ番号は0~79。ディスクの外側が0
		MOV		DH,0	; ヘッド番号は0~1。ディスクの表が0、裏が1
		MOV		CL,2	; セクタ番号は0~18。ディスクの円を18分割したもので1セクタ512バイト

readloop:
		MOV		SI,0	; 失敗回数

retry:
		MOV		AH,0x02	; 読み込み
		MOV		AL,1
		MOV		BX,0
		MOV		DL,0x00	; 現在ではフロッピーディスクドライブは2つ以上存在することはほぼない
		INT		0x13	; エラー時はCF(キャリーフラグ)が1になる
		JNC		next	; エラーがなければ終了
		ADD		SI,1
		CMP		SI,5
		JAE		error
		MOV		AH,0x00	; リセット
		MOV		DL,0x00
		INT		0x13
		JMP		retry

next:
		MOV		AX,ES
		ADD		AX,0x0020	; 512バイト分アドレスを進める
		MOV		ES,AX	; ADD ES,0x0020 命令はない
		ADD		CL,1
		CMP		CL,18	; セクタの最後
		JBE		readloop


fin:
		HLT						; 何かあるまでCPUを停止させる
		JMP		fin				; 無限ループ

error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SIに1を足す
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 一文字表示ファンクション
		MOV		BX,15			; カラーコード
		INT		0x10			; ビデオBIOS呼び出し
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; 改行を2つ
		DB		"load error"
		DB		0x0a			; 改行
		DB		0

		TIMES	0x1fe-($-$$) DB 0		; 0x7dfeまでを0x00で埋める命令

		DB		0x55, 0xaa
