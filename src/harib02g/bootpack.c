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

void init_palette(void);

void backfill8(unsigned char *, int, unsigned char, int, int, int, int);

void set_palette(int start, int end, unsigned char *rgb);

void init_screen(unsigned char *vram, int xsize, int ysize);

void putfont8_asc(char *vram, int xsize, int x, int y, char c,
                  unsigned char *s);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);

// hankaku.txt から生成
// ASCIIに準じて256文字
// A は hankaku + 0x41 * 16 からの16バイトに入っている
extern char hankaku[4096];

typedef struct _BOOTINFO {
  char cyls, leds, vmode, reserve;
  short scrnx, scrny;
  char *vram;
} BOOTINFO;

void HariMain(void) {
  BOOTINFO *binfo;
  char s[40];

  init_palette();
  binfo = (BOOTINFO *)0xff0;

  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  putfont8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, "ABC 123");
  putfont8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "Haribote OS.");
  putfont8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "Haribote OS.");

  sprintf(s, "scrnx = %d", binfo->scrnx);
  putfont8_asc(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF, s);

  for (;;) {
    io_hlt();
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

void init_screen(unsigned char *vram, int xsize, int ysize) {
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