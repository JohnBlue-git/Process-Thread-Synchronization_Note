#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

void *createThread(void *arg) {
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid;
    if (pthread_create(&tid, NULL, createServer, NULL) != 0) {
        perror("Error: Could not create Server side");
        exit(EXIT_FAILURE);
    }
    
    // pthread_cancel(tid); terminate it in the middle
    
    // pthread_detach(tid);
    // or
    pthread_join(tid, NULL);

    exit(EXIT_SUCCESS);
}
