#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 3
#define NUM_WRITERS 1

int shared_resource = 0;
pthread_rwlock_t rw_lock = PTHREAD_RWLOCK_INITIALIZER;

void *reader_thread(void *arg) {
    int i;
    for (i = 0; i < 5; ++i) {
        pthread_rwlock_rdlock(&rw_lock);
        printf("Reader Thread %ld: Read shared resource: %d\n", (long)pthread_self(), shared_resource);
        pthread_rwlock_unlock(&rw_lock);
        usleep(1000000); // Simulate work
    }
    return NULL;
}

void *writer_thread(void *arg) {
    int i;
    for (i = 0; i < 5; ++i) {
        pthread_rwlock_wrlock(&rw_lock);
        shared_resource++;
        printf("Writer Thread %ld: Wrote to shared resource: %d\n", (long)pthread_self(), shared_resource);
        pthread_rwlock_unlock(&rw_lock);
        usleep(2000000); // Simulate work
    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    int i;

    // Create reader threads
    for (i = 0; i < NUM_READERS; ++i) {
        pthread_create(&readers[i], NULL, reader_thread, NULL);
    }

    // Create writer threads
    for (i = 0; i < NUM_WRITERS; ++i) {
        pthread_create(&writers[i], NULL, writer_thread, NULL);
    }

    // Join reader threads
    for (i = 0; i < NUM_READERS; ++i) {
        pthread_join(readers[i], NULL);
    }

    // Join writer threads
    for (i = 0; i < NUM_WRITERS; ++i) {
        pthread_join(writers[i], NULL);
    }

    return 0;
}
