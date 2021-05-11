/* bootpackのメイン */

#include "bootpack.h"
#include <stdio.h>

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000
#define MEMMAN_FREES 4040      // 32KB
#define MEMMAN_ADDR 0x003c0000 // 32KB

struct FREEINFO {
  unsigned int addr, size;
};

struct MEMMAN {
  int frees, maxfrees, losts, lostsize;
  struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);

void memman_init(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
unsigned int memman_total(struct MEMMAN *man);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);

void HariMain(void) {
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
  char s[40], mcursor[256], keybuf[32], mousebuf[128];
  // マウスの座標。左上が(mx, my) = (0, 0)
  int mx, my, i;
  struct MOUSE_DEC mdec;
  unsigned int memtotal;
  struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;

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

  memtotal = memtest(0x00400000, 0xbfffffff);
  memman_init(memman);
  memman_free(memman, 0x00001000, 0x0009e000);
  memman_free(memman, 0x00400000, memtotal - 0x00400000);

  sprintf(s, "memory %dMB free: %dKB", memtotal / (1024 * 1024),
          memman_total(memman) / 1024);
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

void memman_init(struct MEMMAN *man) {
  man->frees = 0;
  man->maxfrees = 0;
  man->losts = 0;
  man->lostsize = 0;
  return;
}

unsigned int memman_total(struct MEMMAN *man) {
  unsigned int i, t = 0;
  for (i = 0; i < man->frees; i++) {
    t += man->free[i].size;
  }
  return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size) {
  unsigned int i, a;
  for (i = 0; i < man->frees; i++) {
    if (man->free[i].size >= size) {
      a = man->free[i].addr;
      man->free[i].addr += size;
      man->free[i].size -= size;
      //
      if (man->free[i].size == 0) {
        man->frees--;
        for (; i < man->frees; i++) {
          man->free[i] = man->free[i + 1];
        }
      }
      return a;
    }
  }
  // 空がない
  return 0;
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size) {
  int i, j;
  for (i = 0; i < man->frees; i++) {
    if (man->free[i].addr > addr) {
      break;
    }
  }
  if (i > 0) {
    if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
      // 前をまとめる
      man->free[i - 1].size += size;
      if (i < man->frees) {
        // 後ろをまとめる
        if (addr + size == man->free[i].addr) {
          man->free[i - 1].size += man->free[i].size;
          man->frees--;
          for (; i < man->frees; i++) {
            man->free[i] = man->free[i + 1];
          }
        }
      }
      // 成功
      return 0;
    }
  }
  if (i < man->frees) {
    // 前はまとめられなかった
    if (addr + size == man->free[i].addr) {
      // 後ろをまとめる
      man->free[i].addr = addr;
      man->free[i].size += size;
      return 0; // 成功
    }
  }
  if (man->frees < MEMMAN_FREES) {
    // 前にも後ろにもまとめられない
    for (j = man->frees; j > i; j--) {
      man->free[j] = man->free[j - 1];
    }
    man->frees++;
    if (man->maxfrees < man->frees) {
      man->maxfrees = man->frees;
    }
    man->free[i].addr = addr;
    man->free[i].size = size;
    return 0; // 成功
  }
  // ずらせなかった
  man->losts++;
  man->lostsize += size;
  return -1; // 失敗
}
