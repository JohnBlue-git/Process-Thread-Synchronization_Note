#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint32_t version;
    uint32_t value;
} snapshot;

static pthread_mutex_t writer_mutex = PTHREAD_MUTEX_INITIALIZER;
static atomic_uint_fast64_t sequence;
static atomic_uint_fast64_t payload;

static void write_snapshot(uint32_t value)
{
    const int error = pthread_mutex_lock(&writer_mutex);
    if (error != 0) {
        fprintf(stderr, "pthread_mutex_lock failed (%d)\n", error);
        exit(EXIT_FAILURE);
    }

    atomic_fetch_add_explicit(&sequence, 1, memory_order_seq_cst);
    const uint32_t version = value * 2U;
    const uint64_t packed = ((uint64_t)version << 32U) | value;
    atomic_store_explicit(&payload, packed, memory_order_seq_cst);
    atomic_fetch_add_explicit(&sequence, 1, memory_order_seq_cst);

    if (pthread_mutex_unlock(&writer_mutex) != 0) {
        fputs("pthread_mutex_unlock failed\n", stderr);
        exit(EXIT_FAILURE);
    }
}

static snapshot read_snapshot(void)
{
    for (;;) {
        const uint64_t before =
            atomic_load_explicit(&sequence, memory_order_seq_cst);
        if ((before & 1U) != 0U) {
            sched_yield();
            continue;
        }
        const uint64_t packed =
            atomic_load_explicit(&payload, memory_order_seq_cst);
        const snapshot copy = {
            .version = (uint32_t)(packed >> 32U),
            .value = (uint32_t)packed,
        };
        const uint64_t after =
            atomic_load_explicit(&sequence, memory_order_seq_cst);
        if (before == after) {
            return copy;
        }
    }
}

static void *writer(void *unused)
{
    (void)unused;
    for (uint32_t value = 1; value <= 10000; ++value) {
        write_snapshot(value);
    }
    return NULL;
}

static void *reader(void *unused)
{
    (void)unused;
    for (int i = 0; i < 1000; ++i) {
        const snapshot copy = read_snapshot();
        if (copy.version != copy.value * 2U) {
            fputs("inconsistent snapshot\n", stderr);
            return (void *)1;
        }
    }
    return NULL;
}

int main(void)
{
    pthread_t writer_thread;
    pthread_t reader_thread;
    int error = pthread_create(&writer_thread, NULL, writer, NULL);
    if (error != 0) {
        fprintf(stderr, "pthread_create failed (%d)\n", error);
        return EXIT_FAILURE;
    }
    error = pthread_create(&reader_thread, NULL, reader, NULL);
    if (error != 0) {
        fprintf(stderr, "pthread_create failed (%d)\n", error);
        pthread_join(writer_thread, NULL);
        return EXIT_FAILURE;
    }

    void *reader_result = NULL;
    error = pthread_join(writer_thread, NULL);
    if (error != 0) {
        fprintf(stderr, "pthread_join failed (%d)\n", error);
        return EXIT_FAILURE;
    }
    error = pthread_join(reader_thread, &reader_result);
    if (error != 0) {
        fprintf(stderr, "pthread_join failed (%d)\n", error);
        return EXIT_FAILURE;
    }
    if (reader_result != NULL) {
        return EXIT_FAILURE;
    }

    const snapshot final = read_snapshot();
    printf("version=%u, value=%u\n", final.version, final.value);
    error = pthread_mutex_destroy(&writer_mutex);
    if (error != 0) {
        fprintf(stderr, "pthread_mutex_destroy failed (%d)\n", error);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
