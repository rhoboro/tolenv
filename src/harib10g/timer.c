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
  timerctl.next = 0xffffffff; // 初期値
  timerctl.using = 0;
  for (i = 0; i < MAX_TIMER; i++) {
    timerctl.timers0[i].flags = 0;
  }
  return;
}

struct TIMER *timer_alloc(void) {
  int i;
  for (i = 0; i < MAX_TIMER; i++) {
    if (timerctl.timers0[i].flags == 0) {
      timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
      return &timerctl.timers0[i];
    }
  }
  // 未使用のものが見つからない場合
  return 0;
}

void timer_free(struct TIMER *timer) {
  timer->flags = 0;
  return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data) {
  timer->fifo = fifo;
  timer->data = data;
  return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout) {
  int e, i, j;
  // 終了時刻
  timer->timeout = timeout + timerctl.count;
  timer->flags = TIMER_FLAGS_USING;

  // 割り込み禁止
  e = io_load_eflags();
  io_cli();

  // 新しいタイマーをどこにいれれば良いか探索
  for (i = 0; i < timerctl.using; ++i) {
    if (timerctl.timers[i]->timeout >= timer->timeout) {
      break;
    }
  }
  // timerctl.timers[i] を入れるために1つずつ以降をずらす
  for (j = timerctl.using; j > i; j--) {
    timerctl.timers[j] = timerctl.timers[j - 1];
  }
  timerctl.using ++;
  // 空いた隙間に挿入
  timerctl.timers[i] = timer;
  timerctl.next = timerctl.timers[0]->timeout;
  io_store_eflags(e);
  return;
}

void inthandler20(int *esp) {
  int i, j;
  io_out8(PIC0_OCW2, 0x60);
  timerctl.count++;
  if (timerctl.next > timerctl.count) {
    return;
  }
  for (i = 0; i < timerctl.using; ++i) {
    if (timerctl.timers[i]->timeout > timerctl.count) {
      // これ以降はすべて動作中
      break;
    }
    timerctl.timers[i]->flags = TIMER_FLAGS_ALLOC;
    fifo32_put(timerctl.timers[i]->fifo, timerctl.timers[i]->data);
  }

  // i は今回タイムアウトしたタイマーの数になっている
  timerctl.using -= i;
  // 残っているタイマーを前に詰める
  for (j = 0; j < timerctl.using; ++j) {
    timerctl.timers[j] = timerctl.timers[i + j];
  }
  if (timerctl.using > 0) {
    timerctl.next = timerctl.timers[0]->timeout;
  } else {
    timerctl.next = 0xffffffff;
  }
  return;
}

// void settimer(struct TIMER timer, struct FIFO32 *fifo, unsigned char data) {
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
