//
// Created by rhoboro on 2021/05/22.
//

#include "bootpack.h"

struct TIMER *mt_timer;
int mt_tr;

void mt_init(void) {
  mt_timer = timer_alloc();
  timer_settime(mt_timer, 2);
  mt_tr = 3 * 8;
  return;
}

// 0.02 秒毎にタスクを切り替える
void mt_taskswitch(void) {
  if (mt_tr == 3 * 8) {
    mt_tr = 4 * 8;
  } else {
    mt_tr = 3 * 8;
  }
  timer_settime(mt_timer, 2);
  farjmp(0, mt_tr);
  return;
}
