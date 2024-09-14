#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>   // For pthread / mutex / spin
#include <semaphore.h> // For semaphore

#define NUM_THREADS 5

// Shared resource
int shared_variable = 0;

// Semaphore
sem_t semaphore;

// Spinlock
pthread_spinlock_t spinlock;

// MCS Spinlock
pthread_mcs_spinlock_t mcs_spinlock = PTHREAD_MCS_SPINLOCK_INITIALIZER;

// Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Example thread
void *example_thread(void *arg) {
    // ...

    // their function are the same
    pthread_exit(NULL);
    return NULL;
    // related discussion:
    //https://stackoverflow.com/questions/3844678/pthread-exit-vs-return
}

// Function for threads using semaphore
void *semaphore_thread(void *arg) {
    int i;
    for (i = 0; i < 5; ++i) {
        sem_wait(&semaphore);
        shared_variable++;
        printf("Semaphore: Thread %ld incremented shared_variable to %d\n", (long)pthread_self(), shared_variable);
        sem_post(&semaphore);
        usleep(100000); // Simulate work
    }
    return NULL;
}

// Function for threads using spinlock
void *spinlock_thread(void *arg) {
    int i;
    for (i = 0; i < 5; ++i) {
        pthread_spin_lock(&spinlock);
        shared_variable++;
        printf("Spinlock: Thread %ld incremented shared_variable to %d\n", (long)pthread_self(), shared_variable);
        pthread_spin_unlock(&spinlock);
        usleep(100000); // Simulate work
    }
    return NULL;
}

// Function for threads using MCS spinlock
void *mcs_spinlock_thread(void *arg) {
    int i;
    for (i = 0; i < 5; ++i) {
        pthread_mcs_spin_lock(&mcs_spinlock);
        shared_variable++;
        printf("MCS Spinlock: Thread %ld incremented shared_variable to %d\n", (long)pthread_self(), shared_variable);
        pthread_mcs_spin_unlock(&mcs_spinlock);
        usleep(100000); // Simulate work
    }
    return NULL;
}

// Function for threads using mutex
void *mutex_thread(void *arg) {
    int i;
    for (i = 0; i < 5; ++i) {
        pthread_mutex_lock(&mutex);
        shared_variable++;
        printf("Mutex: Thread %ld incremented shared_variable to %d\n", (long)pthread_self(), shared_variable);
        pthread_mutex_unlock(&mutex);
        usleep(100000); // Simulate work
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int i;

    // Initialize semaphore
    sem_init(&semaphore, 0, 1);

    // Initialize spinlock
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    // Create threads using semaphore
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, semaphore_thread, NULL);
    }

    // Wait for threads to finish
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Reset shared variable
    shared_variable = 0;

    // Create threads using spinlock
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, spinlock_thread, NULL);
    }

    // Wait for threads to finish
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Reset shared variable
    shared_variable = 0;

    // Create threads using MCS spinlock
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, mcs_spinlock_thread, NULL);
    }

    // Wait for threads to finish
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Reset shared variable
    shared_variable = 0;

    // Create threads using mutex
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, mutex_thread, NULL);
    }

    // Wait for threads to finish
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    sem_destroy(&semaphore);
    pthread_spin_destroy(&spinlock);
    pthread_mcs_spin_destroy(&mcs_spinlock);
    pthread_mutex_destroy(&mutex);

    return 0;
}
