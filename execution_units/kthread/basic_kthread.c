#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/sched.h>

static struct task_struct *worker;

static int worker_fn(void *unused)
{
    (void)unused;
    while (!kthread_should_stop()) {
        pr_info("basic_kthread: worker is running (pid=%d)\n", current->pid);
        ssleep(1);
    }
    return 0;
}

static int __init basic_kthread_init(void)
{
    worker = kthread_run(worker_fn, NULL, "basic_kthread");
    if (IS_ERR(worker)) {
        const int error = PTR_ERR(worker);
        pr_err("basic_kthread: failed to start worker: %d\n", error);
        return error;
    }
    return 0;
}

static void __exit basic_kthread_exit(void)
{
    kthread_stop(worker);
    pr_info("basic_kthread: worker stopped\n");
}

module_init(basic_kthread_init);
module_exit(basic_kthread_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Minimal Linux kernel thread lifecycle example");
