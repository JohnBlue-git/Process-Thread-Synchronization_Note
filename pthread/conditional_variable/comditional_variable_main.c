
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



// Struct to hold the buffer and related synchronization variables
#define BUFFER_SIZE 5
typedef struct {
    int buffer[BUFFER_SIZE];
    int in;  // Index to insert item
    int out; // Index to remove item
    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t full;
} SharedBuffer;

// Initialize the shared buffer
void initBuffer(SharedBuffer *buffer) {
    buffer->in = 0;
    buffer->out = 0;
    pthread_mutex_init(&buffer->mutex, NULL);
    pthread_cond_init(&buffer->empty, NULL);
    pthread_cond_init(&buffer->full, NULL);
}

void clearBuffer(SharedBuffer *buffer) {
    pthread_mutex_destroy(&buffer.mutex);
    pthread_cond_destroy(&buffer.empty);
    pthread_cond_destroy(&buffer.full);
}

// Add an item to the buffer
void addItem(SharedBuffer *buffer, int item) {
    pthread_mutex_lock(&buffer->mutex);
    while ((buffer->in + 1) % BUFFER_SIZE == buffer->out) {
        // Buffer is full, wait for consumer to consume items
        pthread_cond_wait(&buffer->full, &buffer->mutex);
    }
    buffer->buffer[buffer->in] = item;
    buffer->in = (buffer->in + 1) % BUFFER_SIZE;
    pthread_cond_signal(&buffer->empty); // Signal that buffer is no longer empty
    pthread_mutex_unlock(&buffer->mutex);
}

// Remove an item from the buffer
int removeItem(SharedBuffer *buffer) {
    int item;
    pthread_mutex_lock(&buffer->mutex);
    while (buffer->in == buffer->out) {
        // Buffer is empty, wait for producer to add items
        pthread_cond_wait(&buffer->empty, &buffer->mutex);
    }
    item = buffer->buffer[buffer->out];
    buffer->out = (buffer->out + 1) % BUFFER_SIZE;
    pthread_cond_signal(&buffer->full); // Signal that buffer is no longer full
    pthread_mutex_unlock(&buffer->mutex);
    return item;
}




// Producer thread function
void* producer(void *arg) {
    SharedBuffer *buffer = (SharedBuffer*)arg;
    for (int i = 0; i < 10; i++) {
        addItem(buffer, i);
        printf("Produced item: %d\n", i);
    }
    return NULL;
}

// Consumer thread function
void* consumer(void *arg) {
    SharedBuffer *buffer = (SharedBuffer*)arg;
    int item;
    for (int i = 0; i < 10; i++) {
        item = removeItem(buffer);
        printf(" Consumed item: %d\n", item);
    }
    return NULL;
}



int main() {
    pthread_t producer_thread, consumer_thread;
    SharedBuffer buffer;

    // Initialize the shared buffer
    initBuffer(&buffer);

    // Create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, &buffer);
    pthread_create(&consumer_thread, NULL, consumer, &buffer);

    // Wait for threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Cleanup
    clearBuffer(buffer)

    return 0;
}
