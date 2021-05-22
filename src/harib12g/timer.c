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
  struct TIMER *t;
  // http://oswiki.osask.jp/?%28PIT%298254
  // PITの割り込みは IRQ0 に繋がっている
  io_out8(PIT_CTRL, 0x34);
  // 割り込み周期は 11932 (0x2e9c) を指定すると100HZ(10msごと)になる
  // 割り込み周期の下位8bit
  io_out8(PIT_CNT0, 0x9c);
  // 割り込み周期の上位8bit
  io_out8(PIT_CNT0, 0x2e);
  for (i = 0; i < MAX_TIMER; i++) {
    timerctl.timers0[i].flags = 0;
  }
  // ダミー
  t = timer_alloc();
  t->timeout = 0xffffffff;
  t->flags = TIMER_FLAGS_USING;
  t->next = 0;

  timerctl.t0 = t;
  timerctl.count = 0;
  timerctl.next = 0xffffffff; // 初期値
  timerctl.using = 1;
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
  int e;
  struct TIMER *t, *s;
  // 終了時刻
  timer->timeout = timeout + timerctl.count;
  timer->flags = TIMER_FLAGS_USING;

  // 割り込み禁止
  e = io_load_eflags();
  io_cli();

  timerctl.using ++;

  t = timerctl.t0;
  if (timer->timeout <= t->timeout) {
    // 先頭に追加する場合
    timerctl.t0 = timer;
    timer->next = t;
    timerctl.next = timer->timeout;
    io_store_eflags(e);
    return;
  }

  for (;;) {
    s = t;
    t = t->next;
    if (t == 0) {
      break;
    }
    if (timer->timeout <= t->timeout) {
      // s と t の間に入れる場合
      s->next = timer;
      timer->next = t;
      io_store_eflags(e);
      return;
    }
  }
}

void inthandler20(int *esp) {
  int i;
  struct TIMER *timer;
  char ts = 0;
  io_out8(PIC0_OCW2, 0x60);
  timerctl.count++;
  if (timerctl.next > timerctl.count) {
    return;
  }
  // とりあえず先頭を入れる
  timer = timerctl.t0;
  for (i = 0; i < timerctl.using; ++i) {
    if (timer->timeout > timerctl.count) {
      // これ以降はすべて動作中
      break;
    }
    timer->flags = TIMER_FLAGS_ALLOC;
    if (timer != mt_timer) {
      fifo32_put(timer->fifo, timer->data);
    } else {
      ts = 1;
    }
    // nextを見ていく
    timer = timer->next;
  }

  // i は今回タイムアウトしたタイマーの数になっている
  timerctl.t0 = timer;
  timerctl.next = timerctl.t0->timeout;
  if (ts != 0) {
    mt_taskswitch();
  }
  return;
}
