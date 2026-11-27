#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/atomic.h>

static atomic_t counter = ATOMIC_INIT(0);
static struct task_struct *worker;

static int worker_fn(void *unused)
{
    (void)unused;
    while (!kthread_should_stop()) {
        const int value = atomic_inc_return(&counter);
        pr_info("atomic: counter=%d\n", value);
        ssleep(1);
    }
    return 0;
}

static int __init atomic_example_init(void)
{
    worker = kthread_run(worker_fn, NULL, "atomic_example");
    if (IS_ERR(worker)) {
        const int error = PTR_ERR(worker);
        pr_err("atomic: failed to start worker: %d\n", error);
        return error;
    }
    return 0;
}

static void __exit atomic_example_exit(void)
{
    kthread_stop(worker);
}

module_init(atomic_example_init);
module_exit(atomic_example_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel atomic counter example");
