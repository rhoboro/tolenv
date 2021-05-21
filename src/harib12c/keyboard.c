//
// Created by rhoboro on 2021/05/11.
//

#include "bootpack.h"

struct FIFO32 *keyfifo;
int keydata0;

// キーボードコントローラがデータ送信可能になるまで待機
void wait_KBC_sendready(void) {
  for (;;) {
    if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0)
      break;
  }
  return;
}

// IRQ1(キーボード割り込み) は INT 0x21 に送られる
void inthandler21(int *esp) {
  int data;
  // IRQ1の受付完了をPICに通知
  // IRQ番号 + 0x60 を書き込む
  io_out8(PIC0_OCW2, 0x61);
  data = io_in8(PORT_KEYDAT);
  // バッファに格納したらすぐに割り込み処理を終了させる
  fifo32_put(keyfifo, data + keydata0);
  return;
}

void init_keyboard(struct FIFO32 *fifo, int data0) {
  keyfifo = fifo;
  keydata0 = data0;
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, KBC_MODE);
  return;
}
