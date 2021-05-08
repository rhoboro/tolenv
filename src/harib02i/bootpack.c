#include <stdio.h>

#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

void io_hlt(void);

void io_cli(void);

void io_out8(int port, int data);

int io_load_eflags(void);

void io_store_eflags(int eflags);

void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

void init_palette(void);

void backfill8(unsigned char *, int, unsigned char, int, int, int, int);

void set_palette(int start, int end, unsigned char *rgb);

void init_screen8(unsigned char *vram, int xsize, int ysize);

void putfont8_asc(char *vram, int xsize, int x, int y, char c,
                  unsigned char *s);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);

void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0,
                 int py0, char *buf, int bxsize);
void init_gdtidt(void);

// hankaku.txt から生成
// ASCIIに準じて256文字
// A は hankaku + 0x41 * 16 からの16バイトに入っている
extern char hankaku[4096];

typedef struct _BOOTINFO {
  char cyls, leds, vmode, reserve;
  short scrnx, scrny;
  char *vram;
} BOOTINFO;

// GDT(Global Descriptor Table)の中身の8バイト
// GDTにはセグメント情報が格納される
typedef struct _SEGMENT_DESCRIPTOR {
  short limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;

} SEGMENT_DESCRIPTOR;

// IDT(Interrupt Descriptor Table)の中身の8バイト
// IDTには0~255までの割り込み番号ごとに関数を登録するテーブル
typedef struct _GATE_DESCRIPTOR {
  short offset_low, selector;
  char dw_count, access_right;
  short offset_high;

} GATE_DESCRIPTOR;

void set_segmdesc(SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

void HariMain(void) {
  BOOTINFO *binfo = (BOOTINFO *)0xff0;

  char s[40];
  // 16 * 16 で描画する
  char mcursor[256];
  int mx, my;

  init_palette();
  init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

  mx = (binfo->scrnx - 16) / 2;
  my = (binfo->scrny - 28 - 16) / 2;

  init_mouse_cursor8(mcursor, COL8_008484);
  putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

  sprintf(s, "(x, y) = (%d, %d)", mx, my);
  putfont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

  for (;;) {
    io_hlt();
  }
}

void init_gdtidt(void) {
  // アドレスは使われていないところを適当に指定(~0x27ffffまで)
  SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *)0x00270000;
  // アドレスは使われていないところを適当に指定(~0x0026ffffまで)
  GATE_DESCRIPTOR *idt = (GATE_DESCRIPTOR *)0x0026f800;
  int i;

  // GDTの初期化
  // セグメント番号は0~8191まで（16ビットのセグメントレジスタの内上位13ビットのみ）
  for (i = 0; i < 8192; ++i) {
    // 初期化処理
    // gdt は8バイトの構造体のポインタなのでアドレスも8バイトずつ増える
    // リミット(セグメントのバイト数-1)は0を指定
    // ベースは0, アクセス権属性は0
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  // 1番目はリミットが4GBなのでメモリ全体
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
  // 2番目はリミットが512KBで0x00280000
  // 0x280000~0x2ffff には bootpack.hrb があり、このためのセグメントになっている
  // bootpack.hrb は ORG 0 にすると実際は0x280000のオフセットで動く
  set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
  load_gdtr(0xffff, 0x00270000);

  // IDTの初期化
  for (i = 0; i < 256; ++i) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idtr(0x7ff, 0x0026f800);
  return;
}

void set_segmdesc(SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base,
                  int ar) {
  if (limit > 0xfffff) {
    ar |= 0x8000; // G_bit = 1
    limit /= 0x1000;
  }
  sd->limit_low = limit & 0xffff;
  sd->base_low = base & 0xffff;
  sd->base_mid = (base >> 16) & 0xff;
  sd->access_right = ar & 0xff;
  sd->limit_high = ((limit >> 15) & 0x0f) | ((ar >> 0) & 0xf0);
  sd->base_high = (base >> 24) & 0xff;
  return;
}

void set_gatedesc(GATE_DESCRIPTOR *gd, int offset, int selector, int ar) {
  gd->offset_low = offset & 0xffff;
  gd->selector = selector;
  gd->dw_count = (ar >> 0) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high = (offset >> 16) & 0xffff;
  return;
}

void init_mouse_cursor8(char *mouse, char bc) {
  static char cursor[16][16] = {
      "**************..", "*OOOOOOOOOOO*...", "*OOOOOOOOOO*....",
      "*OOOOOOOOO*.....", "*OOOOOOOO*......", "*OOOOOOO*.......",
      "*OOOOOOO*.......", "*OOOOOOOO*......", "*OOOO**OOO*.....",
      "*OOO*..*OOO*....", "*OO*....*OOO*...", "*O*......*OOO*..",
      "**........*OOO*.", "*..........*OOO*", "............*OO*",
      ".............***"};
  int x, y;
  for (y = 0; y < 16; y++) {
    for (x = 0; x < 16; x++) {
      if (cursor[y][x] == '*') {
        mouse[y * 16 + x] = COL8_000000;
      }
      if (cursor[y][x] == 'O') {
        mouse[y * 16 + x] = COL8_FFFFFF;
      }
      if (cursor[y][x] == '.') {
        // bc はバックグラウンドカラー
        mouse[y * 16 + x] = bc;
      }
    }
  }
}

void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0,
                 int py0, char *buf, int bxsize) {
  // pxsize, pysaize は描画したい絵の大きさ
  // vxsize はVRAMの大きさ
  // bxsize
  // は描画する1ラインあたりのサイズ。pxsizeと違う場合があるため別途用意している
  int x, y;
  for (y = 0; y < pysize; y++) {
    for (x = 0; x < pxsize; x++) {
      vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
    }
  }
}

void putfont8_asc(char *vram, int xsize, int x, int y, char c,
                  unsigned char *s) {
  for (; *s != 0x00; s++) {
    putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
    x += 8;
  }
  return;
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font) {
  int i;
  char *p, d;
  for (i = 0; i < 16; i++) {
    d = font[i];
    p = vram + (y + i) * xsize + x;
    if ((d & 0x80) != 0) {
      p[0] = c;
    }
    if ((d & 0x40) != 0) {
      p[1] = c;
    }
    if ((d & 0x20) != 0) {
      p[2] = c;
    }
    if ((d & 0x10) != 0) {
      p[3] = c;
    }
    if ((d & 0x08) != 0) {
      p[4] = c;
    }
    if ((d & 0x04) != 0) {
      p[5] = c;
    }
    if ((d & 0x02) != 0) {
      p[6] = c;
    }
    if ((d & 0x01) != 0) {
      p[7] = c;
    }
  }
  return;
}

void init_screen8(unsigned char *vram, int xsize, int ysize) {
  // 背景
  backfill8(vram, 320, COL8_008484, 0, 0, xsize - 1, ysize - 29);
  backfill8(vram, 320, COL8_C6C6C6, 0, ysize - 28, xsize - 1, ysize - 28);
  backfill8(vram, 320, COL8_FFFFFF, 0, ysize - 27, xsize - 1, ysize - 27);
  backfill8(vram, 320, COL8_C6C6C6, 0, ysize - 26, xsize - 1, ysize - 1);

  // 左下のボタン
  backfill8(vram, 320, COL8_FFFFFF, 3, ysize - 24, 59, ysize - 24);
  backfill8(vram, 320, COL8_FFFFFF, 2, ysize - 24, 2, ysize - 4);
  backfill8(vram, 320, COL8_848484, 3, ysize - 4, 59, ysize - 4);
  backfill8(vram, 320, COL8_848484, 59, ysize - 23, 59, ysize - 5);
  backfill8(vram, 320, COL8_000000, 2, ysize - 3, 59, ysize - 3);
  backfill8(vram, 320, COL8_000000, 60, ysize - 24, 60, ysize - 3);

  // 右下の枠
  backfill8(vram, 320, COL8_848484, xsize - 47, ysize - 24, xsize - 4,
            ysize - 24);
  backfill8(vram, 320, COL8_848484, xsize - 47, ysize - 23, xsize - 47,
            ysize - 4);
  backfill8(vram, 320, COL8_FFFFFF, xsize - 47, ysize - 3, xsize - 4,
            ysize - 3);
  backfill8(vram, 320, COL8_FFFFFF, xsize - 3, ysize - 24, xsize - 3,
            ysize - 3);
}

void backfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0,
               int x1, int y1) {
  int x, y;
  for (y = y0; y <= y1; y++) {
    for (x = x0; x <= x1; x++) {
      vram[y * xsize + x] = c;
    }
  }
}

void init_palette(void) {
  static unsigned char table_rgb[16 * 3] = {
      0x00, 0x00, 0x00, /*  0:黒 */
      0xff, 0x00, 0x00, /*  1:明るい赤 */
      0x00, 0xff, 0x00, /*  2:明るい緑 */
      0xff, 0xff, 0x00, /*  3:明るい黄色 */
      0x00, 0x00, 0xff, /*  4:明るい青 */
      0xff, 0x00, 0xff, /*  5:明るい紫 */
      0x00, 0xff, 0xff, /*  6:明るい水色 */
      0xff, 0xff, 0xff, /*  7:白 */
      0xc6, 0xc6, 0xc6, /*  8:明るい灰色 */
      0x84, 0x00, 0x00, /*  9:暗い赤 */
      0x00, 0x84, 0x00, /* 10:暗い緑 */
      0x84, 0x84, 0x00, /* 11:暗い黄色 */
      0x00, 0x00, 0x84, /* 12:暗い青 */
      0x84, 0x00, 0x84, /* 13:暗い紫 */
      0x00, 0x84, 0x84, /* 14:暗い水色 */
      0x84, 0x84, 0x84  /* 15:暗い灰色 */
  };
  set_palette(0, 15, table_rgb);
  return;

  /* static char 命令は、データにしか使えないけどDB命令相当 */
}

void set_palette(int start, int end, unsigned char *rgb) {
  int i, eflags;
  eflags = io_load_eflags(); /* 割り込み許可フラグの値を記録する */
  io_cli(); /* 許可フラグを0にして割り込み禁止にする */
  io_out8(0x03c8, start);
  for (i = start; i <= end; i++) {
    io_out8(0x03c9, rgb[0] / 4);
    io_out8(0x03c9, rgb[1] / 4);
    io_out8(0x03c9, rgb[2] / 4);
    rgb += 3;
  }
  io_store_eflags(eflags); /* 割り込み許可フラグを元に戻す */
  return;
}
