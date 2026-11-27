#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { THREAD_COUNT = 4, INCREMENTS = 10000 };

static pthread_spinlock_t counter_lock;
static int counter;

static void check_pthread(int error, const char *operation)
{
    if (error != 0) {
        fprintf(stderr, "%s: %s\n", operation, strerror(error));
        exit(EXIT_FAILURE);
    }
}

static void *increment(void *unused)
{
    (void)unused;
    for (int i = 0; i < INCREMENTS; ++i) {
        check_pthread(pthread_spin_lock(&counter_lock), "pthread_spin_lock");
        ++counter;
        check_pthread(pthread_spin_unlock(&counter_lock), "pthread_spin_unlock");
    }
    return NULL;
}

int main(void)
{
    pthread_t threads[THREAD_COUNT];
    check_pthread(pthread_spin_init(&counter_lock, PTHREAD_PROCESS_PRIVATE),
                  "pthread_spin_init");
    for (int i = 0; i < THREAD_COUNT; ++i) {
        check_pthread(pthread_create(&threads[i], NULL, increment, NULL),
                      "pthread_create");
    }
    for (int i = 0; i < THREAD_COUNT; ++i) {
        check_pthread(pthread_join(threads[i], NULL), "pthread_join");
    }
    check_pthread(pthread_spin_destroy(&counter_lock), "pthread_spin_destroy");
    printf("counter=%d (expected %d)\n", counter, THREAD_COUNT * INCREMENTS);
    return EXIT_SUCCESS;
}
