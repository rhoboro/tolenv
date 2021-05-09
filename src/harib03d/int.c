//
// Created by rhoboro on 2021/05/09.
//

#include "bootpack.h"

// PICの初期化
// PICはCPUに割り込みを伝えるコントローラ
// CPUは1つしか割り込み処理ができないのでIPCが集約して伝えている
// CPUとは物理的に繋がっていて、マスタPICから8本の線(0-7)、スレーブPICから8本(8-15)の線がある
// いずれかの線に割り込み信号が来たらそれがCPUに伝わる
// マスタの2番がスレーブに繋がっている
// ここではPIC0がマスタ、PIC1がスレーブ
// 書き込み順にも意味があり、同じポートに書き込んでいるが意図した（IPC内の）レジスタに書き込まれている
void init_pic(void) {
  io_out8(PIC0_IMR, 0xff); /* すべての割り込みを受け付けない */
  io_out8(PIC1_IMR, 0xff); /* すべての割り込みを受け付けない */

  io_out8(PIC0_ICW1, 0x11);   /* エッジトリガモード */
  io_out8(PIC0_ICW2, 0x20);   /* IRQ0-7は、INT20-27で受ける */
  io_out8(PIC0_ICW3, 1 << 2); /* PIC1はIRQ2にて接続 */
  io_out8(PIC0_ICW4, 0x01);   /* ノンバッファモード */

  io_out8(PIC1_ICW1, 0x11); /* エッジトリガモード */
  io_out8(PIC1_ICW2, 0x28); /* IRQ8-15は、INT28-2fで受ける */
  io_out8(PIC1_ICW3, 2);    /* PIC1はIRQ2にて接続 */
  io_out8(PIC1_ICW4, 0x01); /* ノンバッファモード */

  io_out8(PIC0_IMR, 0xfb); /* 11111011 PIC1以外は全て禁止 */
  io_out8(PIC1_IMR, 0xff); /* 11111111 全ての割り込みを受け付けない */

  return;
}