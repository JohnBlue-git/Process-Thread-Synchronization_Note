#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { THREAD_COUNT = 6, PERMITS = 2 };

static sem_t permits;

static void check_pthread(int error, const char *operation)
{
    if (error != 0) {
        fprintf(stderr, "%s: %s\n", operation, strerror(error));
        exit(EXIT_FAILURE);
    }
}

static void wait_for_permit(void)
{
    while (sem_wait(&permits) == -1) {
        if (errno != EINTR) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    }
}

static void *worker(void *argument)
{
    const long id = *(const long *)argument;
    wait_for_permit();
    printf("worker %ld acquired a permit\n", id);
    if (sem_post(&permits) == -1) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
    return NULL;
}

int main(void)
{
    pthread_t threads[THREAD_COUNT];
    long worker_ids[THREAD_COUNT];
    if (sem_init(&permits, 0, PERMITS) == -1) {
        perror("sem_init");
        return EXIT_FAILURE;
    }
    for (long i = 0; i < THREAD_COUNT; ++i) {
        worker_ids[i] = i;
        check_pthread(pthread_create(&threads[i], NULL, worker, &worker_ids[i]),
                      "pthread_create");
    }
    for (int i = 0; i < THREAD_COUNT; ++i) {
        check_pthread(pthread_join(threads[i], NULL), "pthread_join");
    }
    if (sem_destroy(&permits) == -1) {
        perror("sem_destroy");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
