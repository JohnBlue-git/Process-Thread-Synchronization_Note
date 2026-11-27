#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/semaphore.h>

#define WORKER_COUNT 2

static struct semaphore permit;
static struct task_struct *workers[WORKER_COUNT];

static int worker_fn(void *data)
{
    const long id = (long)data;
    while (!kthread_should_stop()) {
        if (down_interruptible(&permit) != 0) {
            continue;
        }
        pr_info("semaphore: worker %ld acquired the permit\n", id);
        up(&permit);
        ssleep(1);
    }
    return 0;
}

static int __init semaphore_example_init(void)
{
    sema_init(&permit, 1);
    for (long i = 0; i < WORKER_COUNT; ++i) {
        workers[i] = kthread_run(worker_fn, (void *)i, "semaphore_%ld", i);
        if (IS_ERR(workers[i])) {
            const int error = PTR_ERR(workers[i]);
            workers[i] = NULL;
            while (--i >= 0) {
                kthread_stop(workers[i]);
            }
            pr_err("semaphore: failed to start worker: %d\n", error);
            return error;
        }
    }
    return 0;
}

static void __exit semaphore_example_exit(void)
{
    for (int i = 0; i < WORKER_COUNT; ++i) {
        if (workers[i] != NULL) {
            kthread_stop(workers[i]);
        }
    }
}

module_init(semaphore_example_init);
module_exit(semaphore_example_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel semaphore example");
