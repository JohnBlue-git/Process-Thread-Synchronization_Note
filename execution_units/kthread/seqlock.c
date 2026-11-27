#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/seqlock.h>

struct sample {
    unsigned int sequence;
    unsigned int value;
};

static DEFINE_SEQLOCK(sample_lock);
static struct sample shared_sample;
static struct task_struct *writer;
static struct task_struct *reader;

static int writer_fn(void *unused)
{
    (void)unused;
    while (!kthread_should_stop()) {
        write_seqlock(&sample_lock);
        ++shared_sample.value;
        shared_sample.sequence = shared_sample.value * 2;
        write_sequnlock(&sample_lock);
        ssleep(1);
    }
    return 0;
}

static int reader_fn(void *unused)
{
    (void)unused;
    while (!kthread_should_stop()) {
        unsigned int seq;
        struct sample copy;
        do {
            seq = read_seqbegin(&sample_lock);
            copy = shared_sample;
        } while (read_seqretry(&sample_lock, seq));
        pr_info("seqlock: sequence=%u value=%u\n", copy.sequence, copy.value);
        ssleep(1);
    }
    return 0;
}

static int __init seqlock_example_init(void)
{
    writer = kthread_run(writer_fn, NULL, "seqlock_writer");
    if (IS_ERR(writer)) {
        const int error = PTR_ERR(writer);
        writer = NULL;
        pr_err("seqlock: failed to start writer: %d\n", error);
        return error;
    }

    reader = kthread_run(reader_fn, NULL, "seqlock_reader");
    if (IS_ERR(reader)) {
        const int error = PTR_ERR(reader);
        reader = NULL;
        kthread_stop(writer);
        writer = NULL;
        pr_err("seqlock: failed to start reader: %d\n", error);
        return error;
    }
    return 0;
}

static void __exit seqlock_example_exit(void)
{
    kthread_stop(reader);
    kthread_stop(writer);
}

module_init(seqlock_example_init);
module_exit(seqlock_example_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel sequence lock snapshot example");
