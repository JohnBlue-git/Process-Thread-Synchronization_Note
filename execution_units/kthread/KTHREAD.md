# Linux kernel threads

A Linux kernel thread (`kthread`) is a kernel task created to run kernel code.
It is not a POSIX thread and does not execute in a user process's address
space. Kernel code has stricter rules than user-space code: for example, code
holding a spin lock must not sleep.

## Examples

Each source is a small, separate loadable module. This directory is independent
of the user-space examples and uses Kbuild rather than the repository's CMake
project:

- [`basic_kthread.c`](basic_kthread.c): start and stop a worker.
- [`mutex.c`](mutex.c): protect sleepable shared state with a kernel mutex.
- [`semaphore.c`](semaphore.c): coordinate workers with a kernel semaphore.
- [`spinlock.c`](spinlock.c): protect a short, non-sleeping critical section.
- [`rwsem.c`](rwsem.c): demonstrate read and write sides of a read-write
  semaphore.
- [`seqlock.c`](seqlock.c): serialize updates and retry readers for a snapshot.
- [`atomic.c`](atomic.c): update an independent integer with `atomic_t`.

The examples use the public kernel interfaces from Linux headers installed for
the target kernel. Kernel APIs may vary by version; these are teaching examples,
not stable out-of-tree module interfaces.

## Build

Install the compiler/build tools and headers matching the kernel that will be
used, then run from this directory:

```sh
make
make clean
```

The Makefile builds all modules against
`/lib/modules/$(uname -r)/build`. If that directory is absent, install the
matching kernel headers or build against an explicitly selected kernel tree.
To build only one module, use `make MODULE=basic_kthread`; set `KDIR` to choose
a different kernel build tree.

## Load and unload one module

[`kthread-module.sh`](kthread-module.sh) builds or manages one allow-listed
module at a time:

```sh
./kthread-module.sh build basic_kthread
sudo ./kthread-module.sh load basic_kthread
./kthread-module.sh status basic_kthread
sudo ./kthread-module.sh unload basic_kthread
```

The script uses `insmod`/`rmmod` and does not automatically force-unload
modules. Build as your normal user; only load and unload actions require root.
Use `KDIR=/path/to/kernel/build` when building against a non-running kernel,
and only load a module built for the kernel that is currently running.

Loading a module changes the running kernel and requires privileges. Inspect
the source and use a disposable virtual machine for experimentation. The
examples should be unloaded after testing; do not load untrusted modules on a
production system.

## Kernel synchronization constraints

- Kernel mutexes and semaphores may block; do not use them in atomic/interrupt
  contexts.
- A spin lock protects only short sections. Do not sleep, allocate with a
  sleeping allocation flag, or call blocking functions while holding it.
- A seqlock is useful when reads can retry and the copied fields are safe to
  read under the kernel's sequence-counter rules. Keep writers serialized.
- `atomic_t` protects an individual atomic value, not arbitrary surrounding
  state.
- A wait queue is the kernel mechanism most similar to a user-space condition
  variable, but it has a different API and usage model.
