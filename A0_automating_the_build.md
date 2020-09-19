# Automating the build

We keep repeating the same steps over and over again whenever we make a change to our code:
* Assemble the boot loader
* Compile the kernel
* Link the kernel
* Generate the OS disk image
* Launch QEMU using the disk image

It's time to use a build tool to help us automate those tasks.

### Make

We'll use GNU make for this purpose. Make allows us to define _targets_ that carry out certain actions. Targets can depend on other targets or files, thus creating a dependency chain.

Our workflow will consist mainly of two tasks for now:
* Build the OS disk image
* Run the OS disk image in QEMU

### Build the OS image

The following Makefile takes care of building our `os.img`.

```makefile
NASM := nasm
GCC := i386-elf-gcc
LD := i386-elf-ld

os.img: bootsect.img kernel.img
	cat bootsect.img kernel.img > os.img

bootsect.img: bootsect.asm
	$(NASM) bootsect.asm -o bootsect.img

kernel.o:
	$(GCC) -fno-asynchronous-unwind-tables -c kernel.c -o kernel.o

kernel.img: kernel.c
	$(GCC) -fno-asynchronous-unwind-tables -c kernel.c -o kernel.o
	$(LD) --oformat=binary --entry=kmain kernel.o -o kernel.img
```

Notice that we cannot use alises in Makefiles, as make doesn't know about shell aliases. We can simplify this a bit by leveraging the automatic variables feature of make.

```makefile
NASM := nasm
GCC := i386-elf-gcc
LD := i386-elf-ld

os.img: bootsect.img kernel.img
	cat $^ > os.img

bootsect.img: bootsect.asm
	$(NASM) $< -o $@

kernel.o: kernel.c
	$(GCC) -fno-asynchronous-unwind-tables -c $< -o $@

kernel.img: kernel.o
	$(LD) --oformat=binary --entry=kmain $^ -o $@
```

The automatic variables we're using are:
* `$^` to represent all prerequisite files
* `$<` to represent the first prerequisite
* `$@` to represent the target name

### Run the OS image in QEMU

To simplify launching QEMU with our OS disk image, let's add a `run` rule.

```makefile
NASM := nasm
GCC := i386-elf-gcc
LD := i386-elf-ld
QEMU := qemu-system-i386

os.img: bootsect.img kernel.img
	cat $^ > os.img

bootsect.img: bootsect.asm
	$(NASM) $< -o $@

kernel.o: kernel.c
	$(GCC) -fno-asynchronous-unwind-tables -c $< -o $@

kernel.img: kernel.o
	$(LD) --oformat=binary --entry=kmain $^ -o $@

run: os.img
	$(QEMU) -nic none -drive file=$<,format=raw
```

Now we can just type `make run` to build everything and launch the built disk image in QEMU.

```
$ make run
i386-elf-gcc -fno-asynchronous-unwind-tables -c kernel.c -o kernel.o
i386-elf-ld --oformat=binary --entry=kmain kernel.o -o kernel.img
cat bootsect.img kernel.img > os.img
qemu-system-i386 -nic none -drive file=os.img,format=raw
```
```
KeaBIOS (version ...)
Booting from Hard Disk...
B
```

That's much easier! The nice thing about make also is that if a target file is up to date, make skips building it unnecessarily.

### Recap

We streamlined our workflow by relying on make to build our kernel and OS image as needed, and launch QEMU using the built disk image.

### Next

It might seem to early to think about loading and running tasks, but that's the main purpose of the operating system: to provide an environment for executing user programs. We'll start thinking about the `Task` abstraction.
