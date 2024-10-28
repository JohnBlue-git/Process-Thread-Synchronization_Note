#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SHM_SIZE  sizeof(int)
#define SHM_KEY   1234

int *shared_memory;
pthread_mutex_t mutex;

void *producer(void *arg) {
    int i;
    for (i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        *shared_memory = i;
        printf("Produced: %d\n", i);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int value;
    while (1) {
        pthread_mutex_lock(&mutex);
        value = *shared_memory;
        printf("Consumed: %d\n", value);
        pthread_mutex_unlock(&mutex);
        if (value == 9)
            break;
        sleep(2);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t producer_thread, consumer_thread;

    // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory
    shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)(-1)) {
        perror("shmat");
        exit(1);
    }

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // Create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Join threads
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Cleanup
    pthread_mutex_destroy(&mutex);
    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
