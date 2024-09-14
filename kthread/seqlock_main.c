#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/seqlock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");

#define NUM_READERS 3
#define NUM_WRITERS 1

static int shared_resource = 0;
static seqlock_t seq_lock = SEQLOCK_UNLOCKED;

int reader_thread(void *data) {
    int i;
    for (i = 0; i < 5; ++i) {
        unsigned int seq;

        // Read lock
        read_seqbegin(&seq_lock, &seq);

        // Read shared resource
        pr_info("Reader Thread %d: Read shared resource: %d\n", current->pid, shared_resource);

        // Check if there was a writer during the read
        if (read_seqretry(&seq_lock, seq)) {
            // Try again or handle the situation
            continue;
        }

        msleep(1000); // Simulate work
    }
    return 0;
}

int writer_thread(void *data) {
    int i;
    for (i = 0; i < 5; ++i) {
        // Write lock
        write_seqlock(&seq_lock);

        // Modify shared resource
        shared_resource++;
        pr_info("Writer Thread %d: Wrote to shared resource: %d\n", current->pid, shared_resource);

        // Release write lock
        write_sequnlock(&seq_lock);

        msleep(2000); // Simulate work
    }
    return 0;
}

static int __init init_seqlock_example(void) {
    int i;
    struct task_struct *reader_threads[NUM_READERS];
    struct task_struct *writer_thread;

    // Create reader threads
    for (i = 0; i < NUM_READERS; ++i) {
        reader_threads[i] = kthread_run(reader_thread, NULL, "reader_thread_%d", i);
    }

    // Create writer thread
    writer_thread = kthread_run(writer_thread, NULL, "writer_thread");

    pr_info("Reader-writer threads created\n");

    return 0;
}

static void __exit cleanup_seqlock_example(void) {
    pr_info("Cleaning up module\n");
}

module_init(init_seqlock_example);
module_exit(cleanup_seqlock_example);
