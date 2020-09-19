NASM := nasm
GCC := i386-elf-gcc
LD := i386-elf-ld
QEMU := qemu-system-i386

CFLAGS := -g -fno-asynchronous-unwind-tables -ffreestanding
LDFLAGS := --oformat=binary --entry=kmain

os.img: bootsect.img kernel.img
	cat $^ > os.img

bootsect.img: bootsect.asm
	$(NASM) $< -o $@

kernel.o: kernel.c
	$(GCC) $(CFLAGS) -c $< -o $@

task.o: task.c
	$(GCC) $(CFLAGS) -c $< -o $@

kernel.img: kernel.o task.o linker.ld
	$(LD) $(LDFLAGS) $^ -T linker.ld -o $@

run: os.img
	$(QEMU) -nic none -drive file=$<,format=raw

clean:
	rm -f *.o *.img
