set(PTHREAD_DEMOS
    atomic
    basic_pthread
    condition_variable
    mutex
    reader_writer
    semaphore
    seqlock
    spinlock
)

foreach(demo IN LISTS PTHREAD_DEMOS)
    add_demo(pthread_${demo} ${demo}.c)
    target_compile_definitions(pthread_${demo} PRIVATE _GNU_SOURCE)
endforeach()
