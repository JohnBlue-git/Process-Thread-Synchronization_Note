#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

enum { THREAD_COUNT = 4, INCREMENTS = 10000 };

static _Atomic int counter;

static void *increment(void *unused)
{
    (void)unused;
    for (int i = 0; i < INCREMENTS; ++i) {
        atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
    }
    return NULL;
}

int main(void)
{
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; ++i) {
        const int error = pthread_create(&threads[i], NULL, increment, NULL);
        if (error != 0) {
            fprintf(stderr, "pthread_create failed (%d)\n", error);
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < THREAD_COUNT; ++i) {
        const int error = pthread_join(threads[i], NULL);
        if (error != 0) {
            fprintf(stderr, "pthread_join failed (%d)\n", error);
            return EXIT_FAILURE;
        }
    }
    printf("counter=%d (expected %d)\n",
           atomic_load_explicit(&counter, memory_order_relaxed),
           THREAD_COUNT * INCREMENTS);
    return EXIT_SUCCESS;
}
