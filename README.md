# Processes, threads, and synchronization

A practical, cross-platform-oriented set of notes and small programs for learning
execution units and synchronization. The examples distinguish user-space APIs
from Linux kernel APIs; they are educational, not production-ready libraries.

## Project map

| Area | Contents |
| --- | --- |
| [Execution units](execution_units/EXECUTION.md) | Processes, threads, OS structures, and platform-specific examples |
| [Synchronization](synchronization/SYNC.md) | Mutexes, semaphores, condition variables, reader-writer locks, seqlocks, and atomics |
| [Process example](execution_units/process/) | POSIX `fork()` and `waitpid()` |
| [POSIX threads](execution_units/pthread/PTHREAD.md) | pthread lifecycle and synchronization examples |
| [C++ threads](execution_units/std-thread/STDTHREAD.md) | `std::thread` and C++ synchronization examples |
| [Linux kernel threads](execution_units/kthread/KTHREAD.md) | Kernel module examples and build guidance |

## Build user-space examples with CMake

The root CMake project builds every process, pthread, C++ thread, and
synchronization example as a separate executable:

```sh
cmake -S . -B build
cmake --build build --parallel
./build/execution_units/pthread/pthread_condition_variable
```

The layered CMake files mirror the source tree: the root project adds
`execution_units/` and `synchronization/`, which add their own example
subdirectories. `execution_units/kthread/` is deliberately not included.

To build only one demo, select its target, such as
`cmake --build build --target synchronization_seqlock`. The POSIX process and
pthread demos require a POSIX-compatible system. The Linux kernel examples are
intentionally excluded from this CMake project; they must be built against
headers matching the target kernel. See
[KTHREAD.md](execution_units/kthread/KTHREAD.md). Do not load an unfamiliar
kernel module on a production system.

## Further reading

The companion project [BasicThreadLock](https://github.com/JohnBlue-git/BasicThreadLock)
contains concise C++ demonstrations of mutexes, spin locks, condition variables,
semaphores, and a bounded producer-consumer scenario. This repository expands
those introductory ideas with lifecycle, memory-model, OS, and kernel context.
