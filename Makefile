.DEFAULT_GOAL := all

NASM := nasm
GCC := i686-elf-gcc
LD := i686-elf-ld
QEMU := "qemu-system-i386"

INCLUDE_DIR=src/include

CFLAGS := -g -fno-asynchronous-unwind-tables -ffreestanding -masm=intel -MMD -MP -mgeneral-regs-only -I$(INCLUDE_DIR)
LDFLAGS := --oformat=binary

SRCDIR := src
BLDDIR := build

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	$(GCC) $(CFLAGS) -c $< -o $@

##
# boot sector
#
$(BLDDIR)/bootsect.bin: $(SRCDIR)/arch_x86/bootsect.asm
	$(NASM) $< -o $@

##
# kernel
#
$(BLDDIR)/kernel/isr.o: $(SRCDIR)/kernel/isr.asm
	$(NASM) -felf32 $< -o $@

$(BLDDIR)/arch_x86/task_switch.o: $(SRCDIR)/arch_x86/task_switch.asm
	$(NASM) -felf32 $< -o $@

KERNEL_LDFLAGS := $(LDFLAGS) --entry=kmain # --print-map

KERNEL_SRCS = \
	$(SRCDIR)/arch_x86/cpu.c \
	$(SRCDIR)/arch_x86/gdt.c \
	$(SRCDIR)/arch_x86/idt.c \
	$(SRCDIR)/arch_x86/port.c \
	$(SRCDIR)/device/ata.c \
	$(SRCDIR)/device/bga.c \
	$(SRCDIR)/device/console.c \
	$(SRCDIR)/device/kbd.c \
	$(SRCDIR)/device/keyboard.c \
	$(SRCDIR)/device/pic.c \
	$(SRCDIR)/device/pit.c \
	$(SRCDIR)/kernel/event.c \
	$(SRCDIR)/kernel/exceptions.c \
	$(SRCDIR)/kernel/interrupt.c \
	$(SRCDIR)/kernel/kernel.c \
	$(SRCDIR)/kernel/loader.c \
	$(SRCDIR)/kernel/scheduler.c \
	$(SRCDIR)/kernel/task.c \
	$(SRCDIR)/kernel/vector.c \
	$(SRCDIR)/lib/blocking_queue.c \
	$(SRCDIR)/lib/queue.c \
	$(SRCDIR)/gui/font.c \
	$(SRCDIR)/gui/gui.c \
	$(SRCDIR)/lib/util.c \
	$(SRCDIR)/shell/shell.c

KERNEL_OBJS = \
	$(patsubst $(SRCDIR)/%.c, $(BLDDIR)/%.o, $(KERNEL_SRCS)) \
	$(BLDDIR)/kernel/isr.o \
	$(BLDDIR)/arch_x86/task_switch.o

KERNEL_DEPS = \
	$(patsubst $(SRCDIR)/%.c, $(BLDDIR)/%.d, $(KERNEL_SRCS))

$(BLDDIR)/kernel.bin: $(KERNEL_OBJS) $(SRCDIR)/kernel/kernel.ld
	$(LD) $(KERNEL_LDFLAGS) $(KERNEL_OBJS) -T $(SRCDIR)/kernel/kernel.ld -o $@

$(shell mkdir -p $(dir $(KERNEL_OBJS)) >/dev/null)

##
# tasks
#
$(BLDDIR)/task_a.bin: $(BLDDIR)/tasks/task_a.o $(SRCDIR)/tasks/task.ld
	$(LD) $(LDFLAGS) $< -T $(SRCDIR)/tasks/task.ld -o $@

$(BLDDIR)/task_b.bin: $(BLDDIR)/tasks/task_b.o $(SRCDIR)/tasks/task.ld
	$(LD) $(LDFLAGS) $< -T $(SRCDIR)/tasks/task.ld -o $@

$(shell mkdir -p $(BLDDIR)/tasks >/dev/null)

##
# OS image
#
# Disk layout:
#   Sector 0:   Boot sector (512 bytes)
#   Sector 1:   File table (512 bytes) - generated at build time
#   Sector 2+:  Kernel
#   After kernel: Tasks, fonts, etc.

# Calculate sector positions dynamically
KERNEL_START_SECTOR := 2
KERNEL_SECTORS = $$(( $$(stat -f%z $(BLDDIR)/kernel.bin) / 512 ))

$(BLDDIR)/filetable.bin: $(BLDDIR)/tools/gen_filetable $(BLDDIR)/kernel.bin $(BLDDIR)/task_a.bin $(BLDDIR)/task_b.bin
	@TASK_A_SECTOR=$$(( $(KERNEL_START_SECTOR) + $$(stat -f%z $(BLDDIR)/kernel.bin) / 512 )); \
	TASK_B_SECTOR=$$(( $$TASK_A_SECTOR + 1 )); \
	echo "Generating file table: task_a=$$TASK_A_SECTOR, task_b=$$TASK_B_SECTOR"; \
	$(BLDDIR)/tools/gen_filetable $@ "task_a:$$TASK_A_SECTOR" "task_b:$$TASK_B_SECTOR"

$(BLDDIR)/os.img: \
	$(BLDDIR)/bootsect.bin \
	$(BLDDIR)/filetable.bin \
	$(BLDDIR)/kernel.bin \
	$(BLDDIR)/task_a.bin \
	$(BLDDIR)/task_b.bin \
	resources/fonts/screen7x14.fon
	cat $^ > $@

##
# Tools
$(BLDDIR)/tools/parse_fon: tools/parse_fon.c
	gcc $< -o $@

$(BLDDIR)/tools/gen_filetable: tools/gen_filetable.c
	gcc $< -o $@

##
# other targets
#
.PHONY: all run clean

all: $(BLDDIR)/os.img

run: $(BLDDIR)/os.img
	$(QEMU) \
        -nic none \
        -drive file=$<,format=raw \
        -display curses \
        -no-shutdown \
        -no-reboot # -d int

run-window: $(BLDDIR)/os.img
	$(QEMU) \
        -nic none \
        -drive file=$<,format=raw \
        -no-reboot & \
	osascript -e 'tell application "System Events" to set frontmost of (first process whose name contains "qemu") to true'

clean:
	$(RM) \
		$(KERNEL_OBJS) \
		$(KERNEL_DEPS) \
		$(BLDDIR)/tasks/task_*.o \
		$(BLDDIR)/tasks/task_*.d \
		$(BLDDIR)/*.bin \
		$(BLDDIR)/os.img \
		$(BLDDIR)/tools/gen_filetable

tools: $(BLDDIR)/tools/parse_fon

$(shell mkdir -p $(BLDDIR)/tools >/dev/null)


-include $(deps)
