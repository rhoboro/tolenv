TOOL_PATH:=../../z_tools

INCPATH:=$(TOOL_PATH)/haribote/
CC1:=$(TOOL_PATH)/gocc1 -Os -Wall -quiet
GAS2NASK:=$(TOOL_PATH)/gas2nask
NASK:=$(TOOL_PATH)/nask
OBJ2BIM:=$(TOOL_PATH)/obj2bim
BIM2HRB:=$(TOOL_PATH)/bim2hrb
MAKEFONT:=$(TOOL_PATH)/makefont
BIN2OBJ:=$(TOOL_PATH)/bin2obj
EDIMG:=$(TOOL_PATH)/edimg
RULEFILE = $(TOOL_PATH)/haribote/haribote.rul

haribote.img: ipl10.bin haribote.sys hello.hrb hello2.hrb hello3.hrb a.hrb crack1.hrb crack2.hrb crack3.hrb crack4.hrb crack5.hrb
	@$(EDIMG) imgin:$(TOOL_PATH)/fdimg0at.tek \
	  wbinimg src:ipl10.bin len:512 from:0 to:0 \
	  copy from:haribote.sys to:@: \
	  copy from:ipl10.nas to:@: \
	  copy from:hello.hrb to:@: \
	  copy from:hello2.hrb to:@: \
	  copy from:hello3.hrb to:@: \
	  copy from:a.hrb to:@: \
	  copy from:crack1.hrb to:@: \
	  copy from:crack2.hrb to:@: \
	  copy from:crack3.hrb to:@: \
	  copy from:crack4.hrb to:@: \
	  copy from:crack5.hrb to:@: \
	  imgout:haribote.img

ipl10.bin: ipl10.nas
	@$(NASK) ipl10.nas ipl10.bin ipl10.lst

haribote.sys : asmhead.bin bootpack.hrb
	@cat asmhead.bin bootpack.hrb > haribote.sys

asmhead.bin: asmhead.nas
	@$(NASK) asmhead.nas asmhead.bin asmhead.lst

bootpack.bim: bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj mtask.obj console.obj window.obj file.obj
	@$(OBJ2BIM) @haribote.rul out:bootpack.bim stack:3136k map:bootpack.map bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj mtask.obj window.obj console.obj file.obj

bootpack.hrb: bootpack.bim
	@$(BIM2HRB) bootpack.bim bootpack.hrb 0

hello.hrb : hello.nas Makefile
	$(NASK) hello.nas hello.hrb hello.lst

hello2.hrb : hello2.nas Makefile
	$(NASK) hello2.nas hello2.hrb hello2.lst

hankaku.bin: hankaku.txt
	@$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj: hankaku.bin
	@$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

hello3.bim: hello3.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello3.bim map:hello3.map hello3.obj a_nask.obj

hello3.hrb: hello3.bim Makefile
	$(BIM2HRB) hello3.bim hello3.hrb 0

crack1.bim: crack1.obj a_nask.obj
	$(OBJ2BIM) @$(RULEFILE) out:crack1.bim map:crack1.map crack1.obj a_nask.obj

crack1.hrb: crack1.bim Makefile
	$(BIM2HRB) crack1.bim crack1.hrb 0

crack2.hrb : crack2.nas Makefile
	$(NASK) crack2.nas crack2.hrb crack2.lst

crack3.hrb : crack3.nas Makefile
	$(NASK) crack3.nas crack3.hrb crack3.lst

crack4.hrb : crack4.nas Makefile
	$(NASK) crack4.nas crack4.hrb crack4.lst

crack5.hrb : crack5.nas Makefile
	$(NASK) crack5.nas crack5.hrb crack5.lst

a.bim: a.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:a.bim map:a.map a.obj a_nask.obj

a.hrb: a.bim Makefile
	$(BIM2HRB) a.bim a.hrb 0

%.gas: %.c Makefile
	@$(CC1) -I$(INCPATH) -o $*.gas $*.c

%.nas: %.gas Makefile
	@$(GAS2NASK) $*.gas $*.nas

%.obj: %.nas
	@$(NASK) $*.nas $*.obj $*.lst


# コマンド
img:
	@$(MAKE) haribote.img

run:
	@$(MAKE) img
	qemu-system-i386 -L . -m 32 -rtc base=localtime -vga std -drive file=haribote.img,index=0,if=floppy,format=raw

clean:
	rm -f *.bin *.sys *.obj *.lst *.img *.bim *.gas *.hrb bootpack.nas bootpack.map

format:
	clang-format -i *.c *.h --style="{ColumnLimit: 120, IndentWidth: 4}"

