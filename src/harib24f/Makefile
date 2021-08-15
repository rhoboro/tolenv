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
OBJS_API = api001.obj api002.obj api003.obj api004.obj api005.obj api006.obj api007.obj api008.obj api009.obj api010.obj api011.obj api012.obj api013.obj api014.obj api015.obj api016.obj api017.obj api018.obj api019.obj api020.obj
GOLIB = $(TOOL_PATH)/golib00

haribote.img: ipl10.bin haribote.sys hello.hrb hello2.hrb hello3.hrb hello4.hrb hello5.hrb a.hrb winhelo.hrb winhelo2.hrb winhelo3.hrb star1.hrb stars.hrb stars2.hrb lines.hrb walk.hrb noodle.hrb beepdown.hrb beepup.hrb color.hrb color2.hrb crack7.hrb
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
	  copy from:lines.hrb to:@: \
	  copy from:walk.hrb to:@: \
	  copy from:noodle.hrb to:@: \
	  copy from:beepdown.hrb to:@: \
	  copy from:beepup.hrb to:@: \
	  copy from:color.hrb to:@: \
	  copy from:color2.hrb to:@: \
	  copy from:crack7.hrb to:@: \
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

hello3.bim: hello3.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello3.bim map:hello3.map hello3.obj apilib.lib

hello3.hrb: hello3.bim Makefile
	$(BIM2HRB) hello3.bim hello3.hrb 0

hello4.bim: hello4.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello4.bim stack:1k map:hello4.map hello4.obj apilib.lib

hello4.hrb: hello4.bim Makefile
	$(BIM2HRB) hello4.bim hello4.hrb 0

hello5.bim : hello5.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello5.bim stack:1k map:hello5.map hello5.obj

hello5.hrb : hello5.bim Makefile
	$(BIM2HRB) hello5.bim hello5.hrb 0

winhelo.bim: winhelo.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo.bim stack:1k map:winhelo.map winhelo.obj apilib.lib

winhelo.hrb: winhelo.bim Makefile
	$(BIM2HRB) winhelo.bim winhelo.hrb 0

winhelo2.bim: winhelo2.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo2.bim stack:1k map:winhelo2.map winhelo2.obj apilib.lib

winhelo2.hrb: winhelo2.bim Makefile
	$(BIM2HRB) winhelo2.bim winhelo2.hrb 0

winhelo3.bim: winhelo3.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo3.bim stack:1k map:winhelo3.map winhelo3.obj apilib.lib

winhelo3.hrb: winhelo3.bim Makefile
	$(BIM2HRB) winhelo3.bim winhelo3.hrb 0

star1.bim: star1.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:star1.bim stack:1k map:star1.map star1.obj apilib.lib

star1.hrb: star1.bim Makefile
	$(BIM2HRB) star1.bim star1.hrb 0

stars.bim: stars.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:stars.bim stack:1k map:stars.map stars.obj apilib.lib

stars.hrb: stars.bim Makefile
	$(BIM2HRB) stars.bim stars.hrb 0

stars2.bim: stars2.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:stars2.bim stack:1k map:stars2.map stars2.obj apilib.lib

stars2.hrb: stars2.bim Makefile
	$(BIM2HRB) stars2.bim stars2.hrb 0

lines.bim: lines.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:lines.bim stack:1k map:lines.map lines.obj apilib.lib

lines.hrb: lines.bim Makefile
	$(BIM2HRB) lines.bim lines.hrb 0

walk.bim: walk.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:walk.bim stack:1k map:walk.map walk.obj apilib.lib

walk.hrb: walk.bim Makefile
	$(BIM2HRB) walk.bim walk.hrb 0

noodle.bim: noodle.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:noodle.bim stack:1k map:noodle.map noodle.obj apilib.lib

noodle.hrb: noodle.bim Makefile
	$(BIM2HRB) noodle.bim noodle.hrb 0

beepdown.bim: beepdown.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:beepdown.bim stack:1k map:beepdown.map beepdown.obj apilib.lib

beepdown.hrb: beepdown.bim Makefile
	$(BIM2HRB) beepdown.bim beepdown.hrb 0

beepup.bim: beepup.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:beepup.bim stack:1k map:beepup.map beepup.obj apilib.lib

beepup.hrb: beepup.bim Makefile
	$(BIM2HRB) beepup.bim beepup.hrb 0

color.bim: color.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:color.bim stack:1k map:color.map color.obj apilib.lib

color.hrb: color.bim Makefile
	$(BIM2HRB) color.bim color.hrb 0

color2.bim: color2.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:color2.bim stack:1k map:color2.map color2.obj apilib.lib

color2.hrb: color2.bim Makefile
	$(BIM2HRB) color2.bim color2.hrb 0

crack7.bim : crack7.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:crack7.bim stack:1k map:crack7.map crack7.obj

crack7.hrb : crack7.bim Makefile
	$(BIM2HRB) crack7.bim crack7.hrb 0k

a.bim: a.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:a.bim map:a.map a.obj apilib.lib

a.hrb: a.bim Makefile
	$(BIM2HRB) a.bim a.hrb 0

apilib.lib: Makefile $(OBJS_API)
	$(GOLIB) $(OBJS_API) out:apilib.lib

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
	rm -f *.bin *.sys *.obj *.lst *.img *.bim *.gas *.hrb bootpack.nas *.map

format:
	clang-format -i *.c *.h --style="{ColumnLimit: 120, IndentWidth: 4}"

