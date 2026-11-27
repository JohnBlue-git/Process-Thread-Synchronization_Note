# Processes and threads

> **A process is an independent program in execution, while a thread is an
> execution stream within a process that shares that process's resources.**

## Concepts

A **process** is a running program with an execution environment managed by the
operating system. It typically has a protected virtual address space, process
credentials, open-resource references, and one or more threads. Processes are
isolated by default; communication between them uses mechanisms such as pipes,
sockets, shared memory, and message queues.

A **thread** is an execution stream scheduled by the OS. Each thread has its own
register state, program counter, stack, and scheduling state. Threads in the
same process normally share the address space and many process resources, such
as code, heap, and open handles/file descriptors.

| Feature | Process | Thread |
| --- | --- | --- |
| Address space | Normally isolated from other processes | Shared with peer threads in its process |
| Execution context | Contains one or more threads | Own registers, stack, and scheduling context |
| Communication | IPC is normally needed to share data | Shared memory, requiring synchronization |
| Failure boundary | Often provides stronger isolation | A fatal error can affect the whole process |
| Creation/switch cost | Often more setup and isolation overhead | Often lighter, but exact costs are OS-dependent |

The OS schedules threads (or a closely related schedulable entity), not source
code functions. Processes are resource and protection containers. A
single-threaded process has one thread; a multithreaded process has several.
"Lightweight process" is used inconsistently and should not be treated as a
portable synonym for thread.

## A simplified relationship

```text
Operating system
├── Process A: address space, credentials, open resources
│   ├── Thread A1: registers + stack + scheduling state
│   └── Thread A2: registers + stack + scheduling state
└── Process B: separate address space and process resources
    └── Thread B1: registers + stack + scheduling state
```

Threads A1 and A2 can directly access shared process memory, but must coordinate
concurrent access. Process A and Process B do not share ordinary private memory
by default.

## Linux and Windows: similar abstractions, different implementation

Both systems provide process isolation and schedulable threads, but their
internal objects and APIs are not interchangeable. The following is an
illustrative, intentionally simplified view; implementation details can change
between OS versions.

```text
Linux (conceptual)                      Windows (conceptual)
------------------                      -------------------
task_struct for each task               EPROCESS process object
  ├─ thread execution state                └─ ETHREAD thread objects
  ├─ mm_struct ─ shared by threads             ├─ per-thread context/stack
  ├─ files_struct ─ shared by threads           └─ process address space
  └─ signal/scheduler state                        and handle table

CLONE_VM / related clone flags           CreateProcess / CreateThread APIs
make selected resources shareable         create processes / threads
```

Linux represents tasks with `task_struct`; threads in a thread group commonly
share selected resources such as the memory descriptor and file table. The
`clone` flags determine which resources are shared. Kernel threads are tasks
without a normal user address space.

Windows exposes process and thread handles through its documented Win32 APIs.
Kernel implementation objects are commonly described as `EPROCESS` and
`ETHREAD`; their layouts are internal and version-dependent. A Windows process
owns an address space and a handle table, while its threads have individual
execution contexts and stacks. Do not write code against undocumented object
layouts.

## User mode and kernel mode

User applications execute with restricted privileges. A system call requests an
OS service and transitions into kernel mode; it does not mean that the calling
program has become a kernel program. Kernel code has broader privileges and
must obey stricter constraints around locking, memory, and blocking.

## Examples in this project

- [POSIX process creation](process/PROCESS.md) and [basic_process.c](process/basic_process.c)
- [POSIX threads](pthread/PTHREAD.md)
- [C++ threads](std-thread/STDTHREAD.md)
- [Linux kernel threads](kthread/KTHREAD.md)

The process example uses POSIX `fork()` and is not a Windows process-creation
example. Windows programs use APIs such as `CreateProcess`; portable C++ thread
examples use the standard library.
