//
// Created by rhoboro on 2021/05/15.
//

#include "bootpack.h"

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040
#define TIMER_FLAGS_ALLOC 1
#define TIMER_FLAGS_USING 2

struct TIMERCTL timerctl;

// Programmable Interval Timer の初期化
void init_pit() {
  int i;
  // http://oswiki.osask.jp/?%28PIT%298254
  // PITの割り込みは IRQ0 に繋がっている
  io_out8(PIT_CTRL, 0x34);
  // 割り込み周期は 11932 (0x2e9c) を指定すると100HZ(10msごと)になる
  // 割り込み周期の下位8bit
  io_out8(PIT_CNT0, 0x9c);
  // 割り込み周期の上位8bit
  io_out8(PIT_CNT0, 0x2e);
  timerctl.count = 0;
  for (i = 0; i < MAX_TIMER; i++) {
    timerctl.timer[i].flags = 0;
  }
  return;
}

struct TIMER *timer_alloc(void) {
  int i;
  for (i = 0; i < MAX_TIMER; i++) {
    if (timerctl.timer[i].flags == 0) {
      timerctl.timer[i].flags = TIMER_FLAGS_ALLOC;
      return &timerctl.timer[i];
    }
  }
  // 未使用のものが見つからない場合
  return 0;
}

void timer_free(struct TIMER *timer) {
  timer->flags = 0;
  return;
}

void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data) {
  timer->fifo = fifo;
  timer->data = data;
  return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout) {
  timer->timeout = timeout;
  timer->flags = TIMER_FLAGS_USING;
  return;
}

void inthandler20(int *esp) {
  // ほぼキーボードと同じ
  int i;
  io_out8(PIC0_OCW2, 0x60);
  timerctl.count++;
  for (i = 0; i < MAX_TIMER; i++) {
    if (timerctl.timer[i].flags == TIMER_FLAGS_USING) {
      timerctl.timer[i].timeout--;
      if (timerctl.timer[i].timeout == 0) {
        timerctl.timer[i].flags = TIMER_FLAGS_ALLOC;
        fifo8_put(timerctl.timer[i].fifo, timerctl.timer[i].data);
      }
    }
  }
  return;
}

// void settimer(struct TIMER timer, struct FIFO8 *fifo, unsigned char data) {
//  int eflags;
//  // 状態保存
//  eflags = io_load_eflags();
//  io_cli(); // この間にIRQ0から割り込みがくると困るので割り込み禁止
//  timer.timeout = timeout;
//  timer.fifo = fifo;
//  timer.data = data;
//  // 状態復元
//  io_store_eflags(eflags);
//  return;
//}
