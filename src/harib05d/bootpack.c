/* bootpackのメイン */

#include "bootpack.h"
#include <stdio.h>

#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KEYCMD_SENDTO_MOUSE 0xd4
#define KBC_MODE 0x47
#define MOUSECMD_ENABLE 0xf4

struct MOUSE_DEC {
  unsigned char buf[3], phase;
  int x, y, btn;
};
extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;
void init_keyboard(void);
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

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
          boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx + 15, my + 15);
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

// キーボードコントローラがデータ送信可能になるまで待機
void wait_KBC_sendready(void) {
  for (;;) {
    if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0)
      break;
  }
  return;
}

void init_keyboard(void) {
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, KBC_MODE);
  return;
}

void enable_mouse(struct MOUSE_DEC *mdec) {
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
  // 0xfa(ACK)を持っている状態
  mdec->phase = 0;
  return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat) {
  if (mdec->phase == 0) {
    // 0xfaはACKなので無視
    if (dat == 0xfa) {
      mdec->phase = 1;
    }
    return 0;
  } else if (mdec->phase == 1) {
    if ((dat & 0xc8) == 0x08) {
      // 正しい1バイト目かどうか
      // 0~3桁で8~Fの範囲にある
      mdec->buf[0] = dat;
      mdec->phase = 2;
    }
    return 0;
  } else if (mdec->phase == 2) {
    mdec->buf[1] = dat;
    mdec->phase = 3;
    return 0;
  } else if (mdec->phase == 3) {
    mdec->buf[2] = dat;
    mdec->phase = 1;
    // ボタンの状態は下位3ビットに収まっている
    // 0x07 は 00000111 なので下位3ビットだけを取得している
    mdec->btn = mdec->buf[0] & 0x07;
    mdec->x = mdec->buf[1];
    mdec->y = mdec->buf[2];
    if ((mdec->buf[0] & 0x10) != 0) {
      // 1バイト目に応じて8ビットより上位のビットを決める
      mdec->x |= 0xffffff00;
    }
    if ((mdec->buf[0] & 0x20) != 0) {
      // 1バイト目に応じて8ビットより上位のビットを決める
      mdec->y |= 0xffffff00;
    }
    mdec->y = -mdec->y;
    return 1;
  }
  // ここには来ないはず
  return -1;
}
