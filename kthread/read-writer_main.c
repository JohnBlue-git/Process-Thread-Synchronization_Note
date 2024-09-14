#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/rwlock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");

#define NUM_READERS 3
#define NUM_WRITERS 1

static int shared_resource = 0;
static rwlock_t rw_lock = RW_LOCK_UNLOCKED;

int reader_thread(void *data) {
    int i;
    for (i = 0; i < 5; ++i) {
        // Acquire read lock
        read_lock(&rw_lock);
        
        // Read shared resource
        pr_info("Reader Thread %d: Read shared resource: %d\n", current->pid, shared_resource);
        
        // Release read lock
        read_unlock(&rw_lock);
        
        msleep(1000); // Simulate work
    }
    return 0;
}

int writer_thread(void *data) {
    int i;
    for (i = 0; i < 5; ++i) {
        // Acquire write lock
        write_lock(&rw_lock);
        
        // Modify shared resource
        shared_resource++;
        pr_info("Writer Thread %d: Wrote to shared resource: %d\n", current->pid, shared_resource);
        
        // Release write lock
        write_unlock(&rw_lock);
        
        msleep(2000); // Simulate work
    }
    return 0;
}

static int __init init_rwlock_example(void) {
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

static void __exit cleanup_rwlock_example(void) {
    pr_info("Cleaning up module\n");
}

module_init(init_rwlock_example);
module_exit(cleanup_rwlock_example);
