# POSIX threads

POSIX threads (`pthreads`) are a C API for creating and coordinating threads
within one process. Threads share process memory and open resources, but each
thread has its own execution stack and thread state.

## Thread lifecycle

[`basic_pthread.c`](basic_pthread.c) creates a worker, passes it an argument,
and joins it so its result is available before the process exits. A joinable
thread should normally be joined or detached; do not return from `main` while
workers still access process state.

## Synchronization demonstrations

- [`mutex.c`](mutex.c): exclusive access with a mutex.
- [`condition_variable.c`](condition_variable.c): bounded queue using a mutex and
  condition variables.
- [`reader_writer.c`](reader_writer.c): concurrent readers and exclusive writer
  using a POSIX read-write lock.
- [`semaphore.c`](semaphore.c): permit counting with an unnamed semaphore.
- [`spinlock.c`](spinlock.c): short user-space critical section using a POSIX
  spin lock where supported.
- [`seqlock.c`](seqlock.c): a retrying snapshot using C11 atomics and a
  writer-serialization mutex.
- [`atomic.c`](atomic.c): independent counter updates with C11
  atomics.

Build an example on a POSIX system from the repository root:

```sh
cmake -S . -B build
cmake --build build --target pthread_condition_variable
./build/execution_units/pthread/pthread_condition_variable
```

POSIX spin locks and unnamed semaphores are not available on every platform.
Check the target platform's documentation. A spin lock is usually a poor choice
for a critical section that may block or take a long time.
