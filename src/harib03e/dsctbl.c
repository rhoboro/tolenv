/* GDTやIDTなどの、 descriptor table 関係 */

#include "bootpack.h"

void init_gdtidt(void) {
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
  struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)ADR_IDT;
  int i;

  /* GDTの初期化 */
  for (i = 0; i < 8192; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
  set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
  load_gdtr(LIMIT_GDT, ADR_GDT);

  /* IDTの初期化 */
  for (i = 0; i < 256; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idtr(LIMIT_IDT, ADR_IDT);

  set_gatedesc(idt + 0x21, (int)asm_inthandler21, 2 * 8, AR_INTGATE32);

  return;
}

// セグメント情報8バイトを書き込む処理
// セグメントの大きさ, 開始アドレス（ベース）, 管理情報
// baseは後方互換のため low, mid, hight に分割された32ビット(=4バイト)の情報
// リミットは20bit。管理属性Gビットフラグを1にするとリミットがバイト単位でなくページ(4KB)単位になる
// これは20bitでバイト単位にすると最大でも1MBまでしか指定できなくなるため
// 32bitOSでのメモリは最大4GB( = 32bitの数値 = 4KB * 1MB = ページ * 20bit)になる
// limit_high
// の上位4bitは管理属性用になるためリミットは2.5バイト(=20bit)しか使えない
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base,
                  int ar) {
  if (limit > 0xfffff) {
    ar |= 0x8000; /* G_bit = 1 */
    limit /= 0x1000;
  }
  sd->limit_low = limit & 0xffff;
  sd->base_low = base & 0xffff;
  sd->base_mid = (base >> 16) & 0xff;
  // 下位8bitが古くからあるセグメント属性
  // 0x92はシステム専用の読み書き可能なセグメント。実行不可。
  // 0x9aはシステム専用の読み込み、実行可能なセグメント。書き込み不可。
  // プログラムが0x9aのセグメントにあるとシステムモード
  // 0xfaのセグメントにあるとアプリケーションモードになる
  sd->access_right = ar & 0xff;
  sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  sd->base_high = (base >> 24) & 0xff;
  return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector,
                  int ar) {
  gd->offset_low = offset & 0xffff;
  gd->selector = selector;
  gd->dw_count = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high = (offset >> 16) & 0xffff;
  return;
}
