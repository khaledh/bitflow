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
src/arch_x86/bootsect.bin: src/arch_x86/bootsect.asm
	$(NASM) $< -o $@

##
# kernel
#
src/arch_x86/task_switch.o: src/arch_x86/task_switch.asm
	$(NASM) -felf32 $< -o $@

KERNEL_LDFLAGS := $(LDFLAGS) --entry=kmain # --print-map

KERNEL_SRCS = \
	src/arch_x86/cpu.c \
	src/arch_x86/idt.c \
	src/arch_x86/port.c \
	src/device/ata.c \
	src/device/console.c \
	src/device/kbd.c \
	src/device/keyboard.c \
	src/device/pic.c \
	src/device/pit.c \
	src/kernel/kvector.c \
	src/kernel/loader.c \
	src/kernel/scheduler.c \
	src/kernel/task.c \
	src/kernel/util.c \
	src/kernel/kernel.c \
	src/shell/shell.c
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o) src/arch_x86/task_switch.o
KERNEL_DEPS = $(KERNEL_SRCS:.c=.d)

kernel.bin: $(KERNEL_OBJS) src/kernel/kernel.ld
	$(LD) $(KERNEL_LDFLAGS) $(KERNEL_OBJS) -T src/kernel/kernel.ld -o $@

##
# tasks
#
task_a.bin: src/tasks/task_a.o src/tasks/task.ld
	$(LD) $(LDFLAGS) $< -T src/tasks/task.ld -o $@

task_b.bin: src/tasks/task_b.o src/tasks/task.ld
	$(LD) $(LDFLAGS) $< -T src/tasks/task.ld -o $@

##
# OS image

os.img: src/arch_x86/bootsect.bin kernel.bin task_a.bin task_b.bin
	cat $^ > os.img

##
# other targets
#
.PHONY: all run clean

all: os.img

run: os.img
	$(QEMU) -nic none -drive file=$<,format=raw -monitor stdio -no-shutdown -no-reboot # -d int

clean:
	$(RM) $(KERNEL_OBJS) $(KERNEL_DEPS) src/tasks/task_*.o src/tasks/task_*.d src/arch_x86/*.bin *.bin os.img

-include $(deps)
