//
// Created by rhoboro on 2021/05/11.
//

#include "bootpack.h"

struct FIFO32 *mousefifo;
int mousedata0;

void inthandler2c(int *esp) {
  int data;
  io_out8(PIC1_OCW2, 0x64);
  io_out8(PIC0_OCW2, 0x62);
  data = io_in8(PORT_KEYDAT);
  fifo32_put(mousefifo, data + mousedata0);
  return;
}

void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec) {
  mousefifo = fifo;
  mousedata0 = data0;
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
