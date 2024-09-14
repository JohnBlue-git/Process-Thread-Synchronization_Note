#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/mcs_spinlock.h>
#include <linux/rwlock.h>
#include <linux/seqlock.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");

// Global variables to be accessed by multiple threads
static int shared_variable = 0;
static DEFINE_SPINLOCK(spinlock);
static struct mcs_spinlock mcs_lock = MCS_SPINLOCK_INITIALIZER;
static rwlock_t rw_lock = RW_LOCK_UNLOCKED;
static seqlock_t seq_lock = SEQLOCK_UNLOCKED;
static struct semaphore sem;
static struct mutex mutex;

// Function for the kernel thread using spinlock
int spinlock_thread(void *data) {
    int i;
    for (i = 0; i < 5; ++i) {
        spin_lock(&spinlock);
        shared_variable++;
        pr_info("Spinlock: %d\n", shared_variable);
        spin_unlock(&spinlock);
        msleep(1000);
    }
    return 0;
}

// Function for the kernel thread using MCS spinlock
int mcs_spinlock_thread(void *data) {
    int i;
    for (i = 0; i < 5; ++i) {
        mcs_spin_lock(&mcs_lock);
        shared_variable++;
        pr_info("MCS Spinlock: %d\n", shared_variable);
        mcs_spin_unlock(&mcs_lock);
        msleep(1000);
    }
    return 0;
}

// Function for the kernel thread using semaphore
int semaphore_thread(void *data) {
    int i;
    for (i = 0; i < 5; ++i) {
        down(&sem);
        shared_variable++;
        pr_info("Semaphore: %d\n", shared_variable);
        up(&sem);
        msleep(1000);
    }
    return 0;
}

// Function for the kernel thread using mutex
int mutex_thread(void *data) {
    int i;
    for (i = 0; i < 5; ++i) {
        mutex_lock(&mutex);
        shared_variable++;
        pr_info("Mutex: %d\n", shared_variable);
        mutex_unlock(&mutex);
        msleep(1000);
    }
    return 0;
}

static int __init init_thread_example(void) {
    struct task_struct *thread1, *thread2, *thread3, *thread4, *thread5, *thread6;

    // Initialize semaphore and mutex
    sema_init(&sem, 1);
    mutex_init(&mutex);

    // Create kernel threads
    thread1 = kthread_run(spinlock_thread, NULL, "spinlock_thread");
    thread2 = kthread_run(mcs_spinlock_thread, NULL, "mcs_spinlock_thread");
    thread5 = kthread_run(semaphore_thread, NULL, "semaphore_thread");
    thread6 = kthread_run(mutex_thread, NULL, "mutex_thread");

    pr_info("Kernel threads created\n");

    return 0;
}

static void __exit cleanup_thread_example(void) {
    pr_info("Cleaning up module\n");
}

module_init(init_thread_example);
module_exit(cleanup_thread_example);
