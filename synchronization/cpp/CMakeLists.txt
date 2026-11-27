set(SYNCHRONIZATION_DEMOS
    atomic_patterns
    producer_consumer
    reader_writer
    seqlock
)

foreach(demo IN LISTS SYNCHRONIZATION_DEMOS)
    add_demo(synchronization_${demo} ${demo}.cpp)
endforeach()
