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

haribote.img: ipl10.bin haribote.sys
	@$(EDIMG) imgin:$(TOOL_PATH)/fdimg0at.tek wbinimg src:ipl10.bin len:512 from:0 to:0 copy from:haribote.sys to:@: imgout:haribote.img

ipl10.bin: ipl10.nas
	@$(NASK) ipl10.nas ipl10.bin ipl10.lst

haribote.sys : asmhead.bin bootpack.hrb
	@cat asmhead.bin bootpack.hrb > haribote.sys

asmhead.bin: asmhead.nas
	@$(NASK) asmhead.nas asmhead.bin asmhead.lst

bootpack.bim: bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj fifo.obj
	@$(OBJ2BIM) @haribote.rul out:bootpack.bim stack:3136k map:bootpack.map bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj fifo.obj

bootpack.hrb: bootpack.bim
	@$(BIM2HRB) bootpack.bim bootpack.hrb 0

hankaku.bin: hankaku.txt
	@$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj: hankaku.bin
	@$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

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
	qemu-system-i386 -drive file=haribote.img,format=raw,if=floppy -boot a

clean:
	rm -f *.bin *.sys *.obj *.lst *.img *.bim *.gas *.hrb bootpack.nas bootpack.map

format:
	clang-format -i *.c *.h

