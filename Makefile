NASM := nasm
GCC := i386-elf-gcc
LD := i386-elf-ld
QEMU := qemu-system-i386

CFLAGS := -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel
LDFLAGS := --oformat=binary --entry=kmain

SRCS = kernel.c console.c cpu.c task.c ata.c task_a.c task_b.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

os.img: bootsect.img kernel.img
	cat $^ > os.img

bootsect.img: bootsect.asm
	$(NASM) $< -o $@

%.o: %.c
	$(GCC) $(CFLAGS) -MMD -MP -c $< -o $@

kernel.img: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -T linker.ld -o $@

run: os.img
	$(QEMU) -nic none -drive file=$<,format=raw -monitor stdio

clean:
	$(RM) $(OBJS) $(DEPS) *.img

-include $(deps)
