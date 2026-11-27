#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check_pthread(int error, const char *operation)
{
    if (error != 0) {
        fprintf(stderr, "%s: %s\n", operation, strerror(error));
        exit(EXIT_FAILURE);
    }
}

static void *worker(void *argument)
{
    const char *message = argument;
    printf("worker received: %s\n", message);
    return argument;
}

int main(void)
{
    pthread_t thread;
    void *result = NULL;
    check_pthread(pthread_create(&thread, NULL, worker, "hello"),
                  "pthread_create");
    check_pthread(pthread_join(thread, &result), "pthread_join");
    printf("joined: %s\n", (const char *)result);
    return EXIT_SUCCESS;
}
