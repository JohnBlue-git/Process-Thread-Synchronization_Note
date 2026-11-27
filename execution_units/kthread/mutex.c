#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/mutex.h>

static DEFINE_MUTEX(value_mutex);
static int shared_value;
static struct task_struct *worker;

static int worker_fn(void *unused)
{
    (void)unused;
    while (!kthread_should_stop()) {
        int snapshot;
        mutex_lock(&value_mutex);
        snapshot = ++shared_value;
        mutex_unlock(&value_mutex);

        pr_info("mutex: value=%d\n", snapshot);
        ssleep(1);
    }
    return 0;
}

static int __init mutex_example_init(void)
{
    worker = kthread_run(worker_fn, NULL, "mutex_example");
    if (IS_ERR(worker)) {
        const int error = PTR_ERR(worker);
        pr_err("mutex: failed to start worker: %d\n", error);
        return error;
    }
    return 0;
}

static void __exit mutex_example_exit(void)
{
    kthread_stop(worker);
}

module_init(mutex_example_init);
module_exit(mutex_example_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel mutex example");
