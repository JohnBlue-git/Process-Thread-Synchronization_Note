# C++ threads

The C++ standard library provides `std::thread` and synchronization primitives
that work across supported platforms. Threads in the same process share memory;
the C++ memory model defines which concurrent operations are safe.

## Lifecycle

[`basic_thread.cpp`](basic_thread.cpp) launches a worker and joins it. A
joinable `std::thread` must be joined or detached before destruction; otherwise
its destructor calls `std::terminate`. Prefer joining unless the task genuinely
has an independent lifetime.

## Synchronization examples

- [`mutex.cpp`](mutex.cpp): mutex with `std::lock_guard`.
- [`condition_variable.cpp`](condition_variable.cpp): bounded queue using a
  condition variable and predicate waits.
- [`shared_mutex.cpp`](shared_mutex.cpp): shared reads and exclusive writes.
- [`semaphore.cpp`](semaphore.cpp): C++20 counting semaphore.
- [`atomic_spinlock.cpp`](atomic_spinlock.cpp): `atomic_flag` as a
  `BasicLockable`; use only for short demonstrations, not as the default mutex.
- [`seqlock.cpp`](seqlock.cpp): retrying atomic snapshot example.

Build an example with CMake from the repository root:

```sh
cmake -S . -B build
cmake --build build --target std_thread_condition_variable
./build/execution_units/std-thread/std_thread_condition_variable
```

Use RAII locks (`lock_guard`, `unique_lock`, `shared_lock`) so exceptions and
early returns do not leave a lock held. Atomics are not a drop-in replacement
for a mutex when an invariant spans multiple fields.
