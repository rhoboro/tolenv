ipl.bin: ipl.nas
	nasm ipl.nas -o ipl.bin -l ipl.lst

haribote.sys : haribote.nas
	nasm haribote.nas -o haribote.sys -l haribote.lst

helloos.img: ipl.bin haribote.sys
	mformat -f 1440 -C -B ipl.bin -i helloos.img ::
	mcopy -i helloos.img haribote.sys ::

run: helloos.img
	qemu-system-i386 -drive file=helloos.img,format=raw,if=floppy -boot a
