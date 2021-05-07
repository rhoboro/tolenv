build:
	nasm helloos.nas -o helloos.img

run:
	qemu-system-i386 -drive file=helloos.img,format=raw,if=floppy -boot a
