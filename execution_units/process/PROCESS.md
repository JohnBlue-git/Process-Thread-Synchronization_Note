# Processes

A process is an OS-managed resource and protection container with at least one
thread of execution. A process ID identifies it within an OS namespace; it is
not a globally permanent identifier.

## POSIX `fork()` example

[`basic_process.c`](basic_process.c) creates one child process. After a
successful `fork()`, parent and child continue from the return point in separate
process contexts. They begin with logically copied memory (commonly implemented
using copy-on-write), but ordinary writes are not shared. The parent waits for
the child and checks its exit status.

Build on a POSIX system from the repository root:

```sh
cmake -S . -B build
cmake --build build --target process_basic_process
./build/execution_units/process/process_basic_process
```

`fork()` is POSIX, not ISO C and not a Windows API. Use `CreateProcess` for
native Windows process creation. For unrelated processes to share data, use an
IPC mechanism rather than assuming ordinary heap or global memory is shared.
