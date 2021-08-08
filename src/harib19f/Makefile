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

haribote.img: ipl10.bin haribote.sys hello.hrb hello2.hrb hello3.hrb hello4.hrb hello5.hrb a.hrb bug1.hrb bug2.hrb bug3.hrb winhelo.hrb
	@$(EDIMG) imgin:$(TOOL_PATH)/fdimg0at.tek \
	  wbinimg src:ipl10.bin len:512 from:0 to:0 \
	  copy from:haribote.sys to:@: \
	  copy from:ipl10.nas to:@: \
	  copy from:hello.hrb to:@: \
	  copy from:hello2.hrb to:@: \
	  copy from:hello3.hrb to:@: \
	  copy from:hello4.hrb to:@: \
	  copy from:hello4.hrb to:@: \
	  copy from:hello5.hrb to:@: \
	  copy from:winhelo.hrb to:@: \
	  copy from:bug1.hrb to:@: \
	  copy from:bug2.hrb to:@: \
	  copy from:bug3.hrb to:@: \
	  copy from:a.hrb to:@: \
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

hello4.bim: hello4.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello4.bim stack:1k map:hello4.map hello4.obj a_nask.obj

hello4.hrb: hello4.bim Makefile
	$(BIM2HRB) hello4.bim hello4.hrb 0

hello5.bim : hello5.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello5.bim stack:1k map:hello5.map hello5.obj

hello5.hrb : hello5.bim Makefile
	$(BIM2HRB) hello5.bim hello5.hrb 0

winhelo.bim: winhelo.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo.bim stack:1k map:winhelo.map winhelo.obj a_nask.obj

winhelo.hrb: winhelo.bim Makefile
	$(BIM2HRB) winhelo.bim winhelo.hrb 0

bug1.bim: bug1.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug1.bim map:bug1.map bug1.obj a_nask.obj

bug1.hrb: bug1.bim Makefile
	$(BIM2HRB) bug1.bim bug1.hrb 0

bug2.bim: bug2.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug2.bim map:bug2.map bug2.obj a_nask.obj

bug2.hrb: bug2.bim Makefile
	$(BIM2HRB) bug2.bim bug2.hrb 0

bug3.bim: bug3.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug3.bim map:bug3.map bug3.obj a_nask.obj

bug3.hrb: bug3.bim Makefile
	$(BIM2HRB) bug3.bim bug3.hrb 0

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

