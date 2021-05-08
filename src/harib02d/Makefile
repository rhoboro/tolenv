clean:
	rm -f *.bin *.sys *.obj *.lst *.img *.bim
format:
	clang-format -i *.c
ipl10.bin: ipl10.nas
	nasm ipl10.nas -o ipl10.bin -l ipl10.lst

helloos.img: ipl10.bin haribote.sys
	mformat -f 1440 -C -B ipl10.bin -i helloos.img ::
	mcopy -i helloos.img haribote.sys ::

run: helloos.img
	qemu-system-i386 -drive file=helloos.img,format=raw,if=floppy -boot a

asmhead.bin:
	@../../z_tools/nask asmhead.nas asmhead.bin asmhead.lst

bootpack.gas: bootpack.c
	@../../z_tools/gocc1 -o bootpack.gas bootpack.c

bootpack.nas: bootpack.gas
	@../../z_tools/gas2nask bootpack.gas bootpack.nas

bootpack.obj: bootpack.nas
	@../../z_tools/nask bootpack.nas bootpack.obj bootpack.lst

bootpack.bim: bootpack.obj naskfunc.obj
	../../z_tools/obj2bim @haribote.rul out:bootpack.bim stack:3136k map:bootpack.map bootpack.obj naskfunc.obj

bootpack.hrb: bootpack.bim
	@../../z_tools/bim2hrb bootpack.bim bootpack.hrb 0

haribote.sys : asmhead.bin bootpack.hrb
	@cat asmhead.bin bootpack.hrb > haribote.sys

naskfunc.obj : naskfunc.nas
	@../../z_tools/nask naskfunc.nas naskfunc.obj naskfunc.lst
