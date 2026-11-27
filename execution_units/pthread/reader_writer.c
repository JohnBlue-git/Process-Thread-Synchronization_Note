#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { READER_COUNT = 3, READS_PER_THREAD = 4, WRITES = 5 };

static pthread_rwlock_t value_lock = PTHREAD_RWLOCK_INITIALIZER;
static int value;

static void check_pthread(int error, const char *operation)
{
    if (error != 0) {
        fprintf(stderr, "%s: %s\n", operation, strerror(error));
        exit(EXIT_FAILURE);
    }
}

static void *reader(void *argument)
{
    const long id = *(const long *)argument;
    for (int i = 0; i < READS_PER_THREAD; ++i) {
        check_pthread(pthread_rwlock_rdlock(&value_lock), "pthread_rwlock_rdlock");
        const int snapshot = value;
        check_pthread(pthread_rwlock_unlock(&value_lock), "pthread_rwlock_unlock");
        printf("reader %ld observed %d\n", id, snapshot);
    }
    return NULL;
}

static void *writer(void *unused)
{
    (void)unused;
    for (int i = 0; i < WRITES; ++i) {
        check_pthread(pthread_rwlock_wrlock(&value_lock), "pthread_rwlock_wrlock");
        ++value;
        check_pthread(pthread_rwlock_unlock(&value_lock), "pthread_rwlock_unlock");
    }
    return NULL;
}

int main(void)
{
    pthread_t readers[READER_COUNT];
    long reader_ids[READER_COUNT];
    pthread_t writer_thread;
    check_pthread(pthread_create(&writer_thread, NULL, writer, NULL), "pthread_create");
    for (long i = 0; i < READER_COUNT; ++i) {
        reader_ids[i] = i;
        check_pthread(pthread_create(&readers[i], NULL, reader, &reader_ids[i]),
                      "pthread_create");
    }
    check_pthread(pthread_join(writer_thread, NULL), "pthread_join");
    for (int i = 0; i < READER_COUNT; ++i) {
        check_pthread(pthread_join(readers[i], NULL), "pthread_join");
    }
    check_pthread(pthread_rwlock_destroy(&value_lock), "pthread_rwlock_destroy");
    printf("final value=%d\n", value);
    return EXIT_SUCCESS;
}
