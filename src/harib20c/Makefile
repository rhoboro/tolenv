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

haribote.img: ipl10.bin haribote.sys hello.hrb hello2.hrb hello3.hrb hello4.hrb hello5.hrb a.hrb winhelo.hrb winhelo2.hrb winhelo3.hrb star1.hrb stars.hrb stars2.hrb
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
	  copy from:winhelo2.hrb to:@: \
	  copy from:winhelo3.hrb to:@: \
	  copy from:star1.hrb to:@: \
	  copy from:stars.hrb to:@: \
	  copy from:stars2.hrb to:@: \
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

winhelo2.bim: winhelo2.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo2.bim stack:1k map:winhelo2.map winhelo2.obj a_nask.obj

winhelo2.hrb: winhelo2.bim Makefile
	$(BIM2HRB) winhelo2.bim winhelo2.hrb 0

winhelo3.bim: winhelo3.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo3.bim stack:1k map:winhelo3.map winhelo3.obj a_nask.obj

winhelo3.hrb: winhelo3.bim Makefile
	$(BIM2HRB) winhelo3.bim winhelo3.hrb 0

star1.bim: star1.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:star1.bim stack:1k map:star1.map star1.obj a_nask.obj

star1.hrb: star1.bim Makefile
	$(BIM2HRB) star1.bim star1.hrb 0

stars.bim: stars.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:stars.bim stack:1k map:stars.map stars.obj a_nask.obj

stars.hrb: stars.bim Makefile
	$(BIM2HRB) stars.bim stars.hrb 0

stars2.bim: stars2.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:stars2.bim stack:1k map:stars2.map stars2.obj a_nask.obj

stars2.hrb: stars2.bim Makefile
	$(BIM2HRB) stars2.bim stars2.hrb 0

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

