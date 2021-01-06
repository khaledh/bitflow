.DEFAULT_GOAL := all

NASM := nasm
GCC := i686-elf-gcc
LD := i686-elf-ld
QEMU := qemu-system-i386

CFLAGS := -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -mgeneral-regs-only
LDFLAGS := --oformat=binary

%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@

##
# boot sector
#
bootsect.bin: bootsect.asm
	$(NASM) $< -o $@

##
# kernel
#
task_switch.o: task_switch.asm
	$(NASM) -felf32 $< -o $@

KERNEL_LDFLAGS := $(LDFLAGS) --entry=kmain # --print-map

KERNEL_SRCS = \
	kernel.c console.c cpu.c task.c \
	port.c ata.c util.c kvector.c \
	idt.c irq.c keyboard.c kbd.c \
	timer.c loader.c shell.c
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o) task_switch.o
KERNEL_DEPS = $(KERNEL_SRCS:.c=.d)

kernel.bin: $(KERNEL_OBJS) kernel.ld
	$(LD) $(KERNEL_LDFLAGS) $(KERNEL_OBJS) -T kernel.ld -o $@

##
# tasks
#
task_a.bin: task_a.o task.ld
	$(LD) $(LDFLAGS) $< -T task.ld -o $@

task_b.bin: task_b.o task.ld
	$(LD) $(LDFLAGS) $< -T task.ld -o $@

##
# OS image

os.img: bootsect.bin kernel.bin task_a.bin task_b.bin
	cat $^ > os.img

##
# other targets
#
.PHONY: all run clean

all: os.img

run: os.img
	$(QEMU) -nic none -drive file=$<,format=raw -monitor stdio -no-shutdown -no-reboot # -d int

clean:
	$(RM) $(KERNEL_OBJS) $(KERNEL_DEPS) task_*.o task_*.d *.bin os.img

-include $(deps)
