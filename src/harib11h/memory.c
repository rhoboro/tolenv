//
// Created by rhoboro on 2021/05/12.
//
#include "bootpack.h"

#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000

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

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size) {
  unsigned int a;
  // & 0x000 で下3桁の切り捨てになる
  // 0xfff
  // を足してから切り捨てると必ず切り上げになる（下3桁が000の時だけ切り上がらない
  // 10進数だと100の場合だけ -> 199 -> 100
  // 101以上は -> 201以上になる -> 200
  size = (size + 0xfff) & 0xfffff000;
  a = memman_alloc(man, size);
  return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size) {
  int i;
  size = (size + 0xfff) & 0xfffff000;
  i = memman_free(man, addr, size);
  return i;
}
