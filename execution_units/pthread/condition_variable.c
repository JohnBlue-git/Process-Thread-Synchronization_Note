#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { CAPACITY = 4, ITEM_COUNT = 12 };

typedef struct {
    int items[CAPACITY];
    int head;
    int tail;
    int count;
    bool finished;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} bounded_queue;

static void check_pthread(int error, const char *operation)
{
    if (error != 0) {
        fprintf(stderr, "%s: %s\n", operation, strerror(error));
        exit(EXIT_FAILURE);
    }
}

static void *produce(void *argument)
{
    bounded_queue *queue = argument;
    for (int item = 1; item <= ITEM_COUNT; ++item) {
        check_pthread(pthread_mutex_lock(&queue->mutex), "pthread_mutex_lock");
        while (queue->count == CAPACITY) {
            check_pthread(pthread_cond_wait(&queue->not_full, &queue->mutex),
                          "pthread_cond_wait");
        }
        queue->items[queue->tail] = item;
        queue->tail = (queue->tail + 1) % CAPACITY;
        ++queue->count;
        printf("produced %d\n", item);
        check_pthread(pthread_cond_signal(&queue->not_empty), "pthread_cond_signal");
        check_pthread(pthread_mutex_unlock(&queue->mutex), "pthread_mutex_unlock");
    }

    check_pthread(pthread_mutex_lock(&queue->mutex), "pthread_mutex_lock");
    queue->finished = true;
    check_pthread(pthread_cond_broadcast(&queue->not_empty),
                  "pthread_cond_broadcast");
    check_pthread(pthread_mutex_unlock(&queue->mutex), "pthread_mutex_unlock");
    return NULL;
}

static void *consume(void *argument)
{
    bounded_queue *queue = argument;
    for (;;) {
        check_pthread(pthread_mutex_lock(&queue->mutex), "pthread_mutex_lock");
        while (queue->count == 0 && !queue->finished) {
            check_pthread(pthread_cond_wait(&queue->not_empty, &queue->mutex),
                          "pthread_cond_wait");
        }
        if (queue->count == 0 && queue->finished) {
            check_pthread(pthread_mutex_unlock(&queue->mutex), "pthread_mutex_unlock");
            break;
        }
        const int item = queue->items[queue->head];
        queue->head = (queue->head + 1) % CAPACITY;
        --queue->count;
        printf("consumed %d\n", item);
        check_pthread(pthread_cond_signal(&queue->not_full), "pthread_cond_signal");
        check_pthread(pthread_mutex_unlock(&queue->mutex), "pthread_mutex_unlock");
    }
    return NULL;
}

int main(void)
{
    bounded_queue queue = {0};
    check_pthread(pthread_mutex_init(&queue.mutex, NULL), "pthread_mutex_init");
    check_pthread(pthread_cond_init(&queue.not_empty, NULL), "pthread_cond_init");
    check_pthread(pthread_cond_init(&queue.not_full, NULL), "pthread_cond_init");

    pthread_t producer;
    pthread_t consumer;
    check_pthread(pthread_create(&producer, NULL, produce, &queue), "pthread_create");
    check_pthread(pthread_create(&consumer, NULL, consume, &queue), "pthread_create");
    check_pthread(pthread_join(producer, NULL), "pthread_join");
    check_pthread(pthread_join(consumer, NULL), "pthread_join");

    check_pthread(pthread_cond_destroy(&queue.not_full), "pthread_cond_destroy");
    check_pthread(pthread_cond_destroy(&queue.not_empty), "pthread_cond_destroy");
    check_pthread(pthread_mutex_destroy(&queue.mutex), "pthread_mutex_destroy");
    return EXIT_SUCCESS;
}
