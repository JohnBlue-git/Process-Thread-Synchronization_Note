#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/spinlock.h>

static DEFINE_SPINLOCK(value_lock);
static int shared_value;
static struct task_struct *worker;

static int worker_fn(void *unused)
{
    (void)unused;
    while (!kthread_should_stop()) {
        unsigned long flags;
        int snapshot;
        spin_lock_irqsave(&value_lock, flags);
        snapshot = ++shared_value;
        spin_unlock_irqrestore(&value_lock, flags);

        pr_info("spinlock: value=%d\n", snapshot);
        ssleep(1);
    }
    return 0;
}

static int __init spinlock_example_init(void)
{
    worker = kthread_run(worker_fn, NULL, "spinlock_example");
    if (IS_ERR(worker)) {
        const int error = PTR_ERR(worker);
        pr_err("spinlock: failed to start worker: %d\n", error);
        return error;
    }
    return 0;
}

static void __exit spinlock_example_exit(void)
{
    kthread_stop(worker);
}

module_init(spinlock_example_init);
module_exit(spinlock_example_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel spin lock example");
