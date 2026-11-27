#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/rwsem.h>

static DECLARE_RWSEM(value_sem);
static int shared_value;
static struct task_struct *worker;

static int worker_fn(void *unused)
{
    (void)unused;
    while (!kthread_should_stop()) {
        int snapshot;
        down_read(&value_sem);
        snapshot = shared_value;
        up_read(&value_sem);
        pr_info("rwsem: read value=%d\n", snapshot);

        down_write(&value_sem);
        ++shared_value;
        up_write(&value_sem);
        ssleep(1);
    }
    return 0;
}

static int __init rwsem_example_init(void)
{
    worker = kthread_run(worker_fn, NULL, "rwsem_example");
    if (IS_ERR(worker)) {
        const int error = PTR_ERR(worker);
        pr_err("rwsem: failed to start worker: %d\n", error);
        return error;
    }
    return 0;
}

static void __exit rwsem_example_exit(void)
{
    kthread_stop(worker);
}

module_init(rwsem_example_init);
module_exit(rwsem_example_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel read-write semaphore example");
