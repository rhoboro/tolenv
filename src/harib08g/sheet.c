/* マウスやウィンドウの重ね合わせ処理 */

#include "bootpack.h"

#define SHEET_USE 1

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram,
                           int xsize, int ysize) {
  struct SHTCTL *ctl;
  int i;
  ctl = (struct SHTCTL *)memman_alloc_4k(memman, sizeof(struct SHTCTL));
  if (ctl == 0) {
    goto err;
  }
  ctl->vram = vram;
  ctl->xsize = xsize;
  ctl->ysize = ysize;
  ctl->top = -1; /* シートは一枚もない */
  for (i = 0; i < MAX_SHEETS; i++) {
    ctl->sheets0[i].flags = 0; /* 未使用マーク */
    ctl->sheets0[i].ctl = ctl;
  }
err:
  return ctl;
}

struct SHEET *sheet_alloc(struct SHTCTL *ctl) {
  struct SHEET *sht;
  int i;
  for (i = 0; i < MAX_SHEETS; i++) {
    if (ctl->sheets0[i].flags == 0) {
      sht = &ctl->sheets0[i];
      sht->flags = SHEET_USE; /* 使用中マーク */
      sht->height = -1;       /* 非表示中 */
      return sht;
    }
  }
  return 0; /* 全てのシートが使用中だった */
}

void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize,
                  int col_inv) {
  sht->buf = buf;
  sht->bxsize = xsize;
  sht->bysize = ysize;
  sht->col_inv = col_inv;
  return;
}

void sheet_updown(struct SHEET *sht, int height) {
  struct SHTCTL *ctl = sht->ctl;
  int h, old = sht->height; /* 設定前の高さを記憶する */

  /* 指定が低すぎや高すぎだったら、修正する */
  if (height > ctl->top + 1) {
    height = ctl->top + 1;
  }
  if (height < -1) {
    height = -1;
  }
  sht->height = height; /* 高さを設定 */

  /* 以下は主にsheets[]の並べ替え */
  if (old > height) { /* 以前よりも低くなる */
    if (height >= 0) {
      /* 間のものを引き上げる */
      for (h = old; h > height; h--) {
        ctl->sheets[h] = ctl->sheets[h - 1];
        ctl->sheets[h]->height = h;
      }
      ctl->sheets[height] = sht;
      sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                       sht->vy0 + sht->bysize, height + 1);
    } else { /* 非表示化 */
      if (ctl->top > old) {
        /* 上になっているものをおろす */
        for (h = old; h < ctl->top; h++) {
          ctl->sheets[h] = ctl->sheets[h + 1];
          ctl->sheets[h]->height = h;
        }
      }
      ctl->top--; /* 表示中の下じきが一つ減るので、一番上の高さが減る */
      sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                       sht->vy0 + sht->bysize, 0);
    }
  } else if (old < height) { /* 以前よりも高くなる */
    if (old >= 0) {
      /* 間のものを押し下げる */
      for (h = old; h < height; h++) {
        ctl->sheets[h] = ctl->sheets[h + 1];
        ctl->sheets[h]->height = h;
      }
      ctl->sheets[height] = sht;
    } else { /* 非表示状態から表示状態へ */
      /* 上になるものを持ち上げる */
      for (h = ctl->top; h >= height; h--) {
        ctl->sheets[h + 1] = ctl->sheets[h];
        ctl->sheets[h + 1]->height = h + 1;
      }
      ctl->sheets[height] = sht;
      ctl->top++; /* 表示中の下じきが一つ増えるので、一番上の高さが増える */
    }
    sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                     sht->vy0 + sht->bysize, height);
  }
  return;
}

void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1) {

  if (sht->height >= 0) {
    sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1,
                     sht->vy0 + by1, sht->height);
  }
  return;
}

// h0より上のシートを再描画する
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1,
                      int h0) {
  int h, bx, by, vx, vy, bx0, by0, bx1, by1;
  unsigned char *buf, c, *vram = ctl->vram;
  struct SHEET *sht;
  if (vx0 < 0)
    vx = 0;
  if (vy0 < 0)
    vy = 0;
  if (vx1 > ctl->xsize)
    vx1 = ctl->xsize;
  if (vy1 > ctl->ysize)
    vy1 = ctl->ysize;

  for (h = h0; h <= ctl->top; h++) {
    sht = ctl->sheets[h];
    buf = sht->buf;
    bx0 = vx0 - sht->vx0;
    by0 = vy0 - sht->vy0;
    bx1 = vx1 - sht->vx0;
    by1 = vy1 - sht->vy0;
    if (bx0 < 0)
      bx0 = 0;
    if (by0 < 0)
      by0 = 0;
    if (bx1 > sht->bxsize)
      bx1 = sht->bxsize;
    if (by1 > sht->bysize)
      by1 = sht->bysize;
    for (by = by0; by < by1; by++) {
      vy = sht->vy0 + by;
      for (bx = bx0; bx < bx1; bx++) {
        vx = sht->vx0 + bx;
        if (vx0 <= vx && vx < vx1 && vy0 <= vy && vy < vy1) {
          c = buf[by * sht->bxsize + bx];
          if (c != sht->col_inv) {
            vram[vy * ctl->xsize + vx] = c;
          }
        }
      }
    }
  }
  return;
}

void sheet_slide(struct SHEET *sht, int vx0, int vy0) {
  int old_vx = sht->vx0, old_vy = sht->vy0;
  sht->vx0 = vx0;
  sht->vy0 = vy0;
  if (sht->height >= 0) { /* もしも表示中なら */
    sheet_refreshsub(sht->ctl, old_vx, old_vy, old_vx + sht->bxsize,
                     old_vy + sht->bysize, 0);
    sheet_refreshsub(sht->ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize,
                     sht->height);
  }
  return;
}

void sheet_free(struct SHEET *sht) {
  if (sht->height >= 0) {
    sheet_updown(sht, -1); /* 表示中ならまず非表示にする */
  }
  sht->flags = 0; /* 未使用マーク */
  return;
}
