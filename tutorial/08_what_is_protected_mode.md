# What is Protected Mode?

Note: Unfortunately this is one of the topics where we can't just jump into coding. There's some essential background we need to learn first.

We're still in 16-bit real mode, which has the following limitations/issues:
  * Total addressable memory is 1MB, only 640KB of which is available to us.
  * There's no way to protect the kernel from user applications, or applications from each other. Any application can crash the kernel by writing over its memory.
  * Hardware access is not restricted to the kernel; any application can access the hardware directly, causing stability issues.
  * Certain instructions that can modify the system state (e.g. halt the CPU) are not restricted to the kernel.

### 16-bit Protected Mode

In addition to supporting rael mode for backward compatibility, the x86 supports another mode called **Protected Mode** that solves all those problems. Protected mode was first introduced in the 16-bit 286, which used 24-bit physical address space (i.e. max 16MB of memory) and 16-bit segment sizes (i.e. max 64K). Although it achives protection, this is still a restricted environment. In addition to the small amount of addressable memory and small segment size, currently there are no mainstream 286 emulators or 16-bit C compilers[^1]. We  will have to move to 32-bit to leverage the available emulators and modern compilers such as GCC or Clang.

[^1]: There are 16-bit DOS C compilers available (e.g. Turbo C or DJGPP), but they need a DOS environment to run in (e.g. DOSBox). This would complicate our workflow significantly and there's not much benefit from going through the trouble.

### 32-bit Protected Mode

With the introduction of the 386, protected mode supported a 32-bit address space, allowing addressing of up to 4GB of memory. Segment sizes were also increased to 32-bit, allowing segments to take up all the 4GB address space, if needed. This latter case is called a "flat memory model", since the segment boundary disappears and all the address space is reachable from within the segment. The x86 now supports a 64-bit mode as well, known as x86_64 or "Long Mode", but we will not concern ourselves with 64-bit for now.

So switching to 32-bit protected mode unlocks a sufficiently large memory address space, and gives us the ability to created arbitrary sized memory segments with hardware-enforced protection boundaries.

### Privilege Levels

In addition, each segment is assigned one of four privilege levels, unofficially called ring 0 to ring 3, where ring 0 is the highest privilege (for the kernel), and rings 1-3 are for less privileged code. Modern operating systems typically use two rings only: ring 0 for kernel mode, and ring 3 for user mode. The CPU enforces strict rules for controlled transfer between the different privilege levels. If you heard the term "syscall" before, this basically means that code running in ring 3 (i.e. user program) wants to invoke a routine in ring 3 (i.e. kernel). This is because user mode should not be able to call the kernel except through well-defined entry points (syscalls).

### Memory Segmenation

If you remember, addressing in real mode uses a segment:offset scheme, where the segment number is multiplied by 16 to get the segment's base address, and then the offset is added to get the final memory address. The fact that the segment number is directly associated with its physical memory location is what gives "real address mode" its name.

In protected mode, an address is still made up of segment:offset pair, but the segment part is not related to a memory location, but rather refers to a **segment descriptor** stored in a well-defined table structure called **Global Descriptor Table** (GDT). The segment descriptor is a 32-bit structure that contains attributes of a segment, such as its base address, size, privilege level, and whether it is a code or data segment, among other things. The kernel is responsible for building the GDT and letting the CPU know where it is located in memory through a special instruction. This is required before we can switch to protected mode, as the CPU will start enforcing those segment boundaries and privileges as soon as we enter protected mode.

Despite the advantages that memory segmentation offers, it ha some draw backs:
* If we create many segments (e.g. at least two for each user program), it can lead to external fragmentation as segments of arbitrary sizes are allocated and freed.
* If we run out of physical memory, we can employ swapping to disk. But with segmentation we cannot swap parts of a segment; we have to swap out an entire segment. This can be costly for large segment sizes, as well as complicate the swapping algorithm.
* Memory management becomes somewhat complicated overall.
* It is not portable to other processor architectures.

For those reasons, modern operating systems shun segmentation in favour of **paging** (we'll cover paging later). That being said, segmentation can actually be used on top of paging. However, the complexity of segmentation makes it not worth the trouble, since paging offers memory protection, virtual memory, efficient swapping, amonth other features. That being said, segmentation is a crucial part of protected mode to facilitate cross privilege level control transfer. To keep its use to a minimum, operating system designers adopt the "flat memory model", where two pairs of code/data flat segments are used: two for kernel mode and two for user mode. All of those segments have a base address of 0x0 and a limit of 0xFFFFFFFF (i.e. 4GB in size), so they cover the entire address space.

One final note: I think segmentation is [underrated](https://softwareengineering.stackexchange.com/a/100088/40956).:
* It allows for hardware-enforced segment limit checks so we can achieve fine-grained memory protection for allocated compoennts/objects (thus avoiding buffer overruns).
* It also makes code relocation simple by definition (code is always executed relative to the segment base address), thus simplifying linkers/loaders.
* The four protection levels allow for more fine-grained access control than with the typical kernel/user mode levels. For example, user mode programs can have more than privilege level, allowing an entire class of memory disclosure security problems to be solved (this is a hot research area with many unofficial methods being proposed to make it possible).

Whether those benefits are worth the complexity/effort to leverage segmentation is a relative matter. But it is undeniable that removal of segmentation in 64-bit Long Mode restricts innovation in the operating system world in this area.