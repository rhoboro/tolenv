ipl10.bin: ipl10.nas
	nasm ipl10.nas -o ipl10.bin -l ipl10.lst

haribote.sys : haribote.nas
	nasm haribote.nas -o haribote.sys -l haribote.lst

helloos.img: ipl10.bin haribote.sys
	mformat -f 1440 -C -B ipl10.bin -i helloos.img ::
	mcopy -i helloos.img haribote.sys ::

run: helloos.img
	qemu-system-i386 -drive file=helloos.img,format=raw,if=floppy -boot a
