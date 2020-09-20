NASM := nasm
GCC := i386-elf-gcc
LD := i386-elf-ld
QEMU := qemu-system-i386

CFLAGS := -g -fno-asynchronous-unwind-tables -ffreestanding
LDFLAGS := --oformat=binary --entry=kmain

OBJECTS := kernel.o cpu.o screen.o task.o task_a.o task_b.o

os.img: bootsect.img kernel.img
	cat $^ > os.img

bootsect.img: bootsect.asm
	$(NASM) $< -o $@

%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@

kernel.img: $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -T linker.ld -o $@

run: os.img
	$(QEMU) -nic none -drive file=$<,format=raw

clean:
	rm -f *.o *.img
