Objective
- It's an example of producer and consumer using (pthread) conditional variable.

Some notes about conditional variable

- Is it a lock ? \
  No, a conditional variable in pthreads is not a lock itself. Instead, it is a synchronization primitive used in conjunction with locks to enable threads to wait for a particular condition to become true before proceeding.

- How it work ? \
A conditional variable lets threads to suspend until another thread "signals" or "broadcasts" to inform that a condition has changed.

- The Linux kernel does not implement conditional variables ! \
In C, it is only exists in pthread (or user space).

When a thread waits on a conditional variable, it goes to sleep, relinquishing the CPU until the condition is signaled. This avoids busy waiting and reduces CPU usage compared to spinning on a semaphore.
```
    while (condtion not match) { // prepare from waking up and condition not match 
        pthread_cond_wait(<cond_var>, <mutex>);
    }
```
