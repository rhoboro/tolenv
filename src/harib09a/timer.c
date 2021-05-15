//
// Created by rhoboro on 2021/05/15.
//

#include "bootpack.h"

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

// Programmable Interval Timer の初期化
void init_pit() {
  // http://oswiki.osask.jp/?%28PIT%298254
  // PITの割り込みは IRQ0 に繋がっている
  io_out8(PIT_CTRL, 0x34);
  // 割り込み周期は 11932 (0x2e9c) を指定すると100HZ(10msごと)になる
  // 割り込み周期の下位8bit
  io_out8(PIT_CNT0, 0x9c);
  // 割り込み周期の上位8bit
  io_out8(PIT_CNT0, 0x2e);
}

void inthandler20(int *esp) {
  // ほぼキーボードと同じ
  io_out8(PIC0_OCW2, 0x60);
  return;
}
