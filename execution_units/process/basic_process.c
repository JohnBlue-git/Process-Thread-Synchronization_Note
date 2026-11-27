#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    fflush(stdout);
    const pid_t child = fork();
    if (child == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child == 0) {
        printf("child process: pid=%ld, parent=%ld\n",
               (long)getpid(), (long)getppid());
        fflush(stdout);
        _exit(EXIT_SUCCESS);
    }

    printf("parent process: pid=%ld, child=%ld\n",
           (long)getpid(), (long)child);

    int status;
    pid_t waited;
    do {
        waited = waitpid(child, &status, 0);
    } while (waited == -1 && errno == EINTR);

    if (waited == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }
    if (WIFEXITED(status)) {
        printf("child exited with status %d\n", WEXITSTATUS(status));
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        fprintf(stderr, "child terminated by signal %d\n", WTERMSIG(status));
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}
