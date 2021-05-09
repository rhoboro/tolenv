//
// Created by rhoboro on 2021/05/09.
//

#include "bootpack.h"
#include <stdio.h>

#define PORT_KEYDAT 0x0060

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

  io_out8(PIC0_ICW1, 0x11); /* エッジトリガモード */
  // IRQ0-15をINT
  // 0x20-0x2fで受け取る。0x00-0x1fはCPUが内部で使うため利用できない
  // IRQ1がキーボードで、これはINT 0x21に送られる
  io_out8(PIC0_ICW2, 0x20);   /* IRQ0-7は、INT20-27で受ける */
  io_out8(PIC0_ICW3, 1 << 2); /* PIC1はIRQ2にて接続 */
  io_out8(PIC0_ICW4, 0x01);   /* ノンバッファモード */

  io_out8(PIC1_ICW1, 0x11); /* エッジトリガモード */
  // IRQ12がマウスで、これはINT 0x2Cに送られる
  io_out8(PIC1_ICW2, 0x28); /* IRQ8-15は、INT28-2fで受ける */
  io_out8(PIC1_ICW3, 2);    /* PIC1はIRQ2にて接続 */
  io_out8(PIC1_ICW4, 0x01); /* ノンバッファモード */

  io_out8(PIC0_IMR, 0xfb); /* 11111011 PIC1以外は全て禁止 */
  io_out8(PIC1_IMR, 0xff); /* 11111111 全ての割り込みを受け付けない */

  return;
}

// IRQ1(キーボード割り込み) は INT 0x21 に送られる
void inthandler21(int *esp) {
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
  unsigned char data, s[4];

  // IRQ1の受付完了をPICに通知
  // IRQ番号 + 0x60 を書き込む
  io_out8(PIC0_OCW2, 0x61);
  data = io_in8(PORT_KEYDAT);

  sprintf(s, "%02X", data);
  boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
  putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
  return;
}

void inthandler2c(int *esp) {
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
  boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
  putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF,
                "INT 2C (IRQ-12) : PS/2 mouse");
  for (;;) {
    io_hlt();
  }
}
