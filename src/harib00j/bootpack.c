void io_hlt(void);

void HariMain(void) {
  fin:
    // naskfunc.nas の _io_hlt() を実行
    io_hlt();
    goto fin;
}
