set(STD_THREAD_DEMOS
    atomic_spinlock
    basic_thread
    condition_variable
    mutex
    semaphore
    seqlock
    shared_mutex
)

foreach(demo IN LISTS STD_THREAD_DEMOS)
    add_demo(std_thread_${demo} ${demo}.cpp)
endforeach()
