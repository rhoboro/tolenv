/* bootpackのメイン */

#include "bootpack.h"
#include <stdio.h>

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000

unsigned int memtest(unsigned int start, unsigned int end);

unsigned int memtest_sub(unsigned int start, unsigned int end);

void HariMain(void) {
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
  char s[40], mcursor[256], keybuf[32], mousebuf[128];
  // マウスの座標。左上が(mx, my) = (0, 0)
  int mx, my, i;
  struct MOUSE_DEC mdec;

  init_gdtidt();
  init_pic();
  io_sti();
  fifo8_init(&keyfifo, 32, keybuf);
  fifo8_init(&mousefifo, 128, mousebuf);
  io_out8(PIC0_IMR, 0xf9); // PIC1とキーボードを許可(11111001)
  io_out8(PIC1_IMR, 0xef); // マウスを許可(11101111)

  init_keyboard();

  init_palette();
  init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);
  mx = (binfo->scrnx - 16) / 2; /* 画面中央になるように座標計算 */
  my = (binfo->scrny - 28 - 16) / 2;
  init_mouse_cursor8(mcursor, COL8_008484);
  putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
  sprintf(s, "(%d, %d)", mx, my);
  putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

  enable_mouse(&mdec);

  i = memtest(0x00400000, 0xbfffffff) / (1024 * 1024);
  sprintf(s, "memory: %dMB", i);
  putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

  for (;;) {
    io_cli();
    if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
      io_stihlt();
    } else {
      if (fifo8_status(&keyfifo) != 0) {
        i = fifo8_get(&keyfifo);
        io_sti();
        sprintf(s, "%02X", i);
        boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
        putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
      } else if (fifo8_status(&mousefifo) != 0) {
        // マウスのデータは3バイトくる
        i = fifo8_get(&mousefifo);
        io_sti();
        if (mouse_decode(&mdec, i) != 0) {
          // データが揃ったので処理を実行
          sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
          if ((mdec.btn & 0x01) != 0) {
            // 1桁目が1ならクリック中で表示の l -> L にする
            s[1] = 'L';
          }
          if ((mdec.btn & 0x02) != 0) {
            // 2桁目が1なら右クリック中で表示の r -> R にする
            s[3] = 'R';
          }
          if ((mdec.btn & 0x04) != 0) {
            s[2] = 'C';
          }
          boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16,
                   32 + 15 * 8 - 1, 31);
          putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);

          // マウスを動かす
          // マウスを消す
          boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx + 15,
                   my + 15);
          mx += mdec.x;
          my += mdec.y;

          // 画面外にはみ出ないように調整
          if (mx < 0) {
            // 左端
            mx = 0;
          }
          if (my < 0) {
            // 上端
            my = 0;
          }
          if (mx > binfo->scrnx - 16) {
            // 右端
            mx = binfo->scrnx - 16;
          }
          if (my > binfo->scrny - 16) {
            // 下端
            my = binfo->scrny - 16;
          }

          // 座標の表示を更新
          sprintf(s, "(%3d, %3d)", mx, my);
          boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15);
          putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

          // マウスを描く
          putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
        }
      }
    }
  }
}

unsigned int memtest(unsigned int start, unsigned int end) {
  char flg486 = 0;
  unsigned int eflg, cr0, i;

  // 18ビット目のACフラグは386だと常にゼロ
  // 386だと1を入れても保存されず0が戻ってくるので1を入れて1が返れば486
  // ただし1が戻ってきても安心できないので反転させてダブルチェックしている
  eflg = io_load_eflags();
  eflg |= EFLAGS_AC_BIT; // AC-bit = 1 にする
  io_store_eflags(eflg);
  eflg = io_load_eflags();
  if ((eflg & EFLAGS_AC_BIT) != 0) { // 386 だと1を入れても常に0になる
    flg486 = 1;
  }
  eflg &= ~EFLAGS_AC_BIT; // 全ビット反転とAND で AC-bit = 0 にする
  io_store_eflags(eflg);

  if (flg486 != 0) {
    cr0 = load_cr0();
    cr0 |= CR0_CACHE_DISABLE; // キャッシュ制御フラグのbitを確実に1にする
    store_cr0(cr0);
  }

  i = memtest_sub(start, end);

  if (flg486 != 0) {
    cr0 = load_cr0();
    cr0 &= ~CR0_CACHE_DISABLE; // キャッシュ制御フラグのbitを確実に0にする
    store_cr0(cr0);
  }
  return i;
}

unsigned int memtest_sub(unsigned int start, unsigned int end) {
  // pat0, pat1
  // を書き込んだあとに取得して同じ値だったらメモリにデータが保存されているとみなせる
  unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
  for (i = start; i <= end; i += 0x1000) {
    p = (unsigned int *)(i + 0xffc); // 末尾の4バイトだけ確認する
    old = *p;                        // もとの値を保存
    *p = pat0;
    *p ^=
        0xffffffff; // 試しに反転してみる. ^ は XORで値が違うところだけが1になる
    if (*p != pat1) {
    not_memory:
      *p = old;
      break;
    }
    *p ^= 0xffffffff; // 再度反転してみる
    if (*p != pat0) {
      goto not_memory;
    }
    *p = old; // もとに戻す
  }
  return i;
}
