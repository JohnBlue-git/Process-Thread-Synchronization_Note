#include <stdio.h>      // For printf and perror
#include <stdlib.h>     // For exit and EXIT_FAILURE/EXIT_SUCCESS
#include <unistd.h>     // For fork, getpid, and waitpid
#include <sys/wait.h>   // For waitpid
#include <sys/types.h>  // For pid_t

void process_context() {
    // ...
}

int main() {
    // Array to hold child process IDs
    pid_t childProcesses[3];
    int numChildren = 0;

    // Fork process
    for (int i = 0; i < 3; ++i) {
        pid_t pid = fork();

        if (pid == 0) { // Child process
            printf("Child!\n");
            process_context();
            exit(0); // Exit child process
        } else if (pid > 0) { // Parent process
            printf("Parent!\n");
            childProcesses[numChildren++] = pid;
        } else { // pid < 0  Fork failed
            perror("Fork failed");
            exit(1);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < numChildren; ++i) {
        waitpid(childProcesses[i], NULL, 0);
    }
    
    // 0 EXIT_SUCCESS
    // 1 EXIT_FAILURE

    // return will clear, exit, and return value.
    // exit() is returned to the operating system as the program's return code or exit code. By convention, a return code of zero means that the program completed successfully. You can use the constants EXIT_FAILURE and EXIT_SUCCESS.
    // abort() terminates the program immediately. The abort function bypasses the normal destruction process for initialized global static objects. It also bypasses any special processing that was specified using the atexit function.
    return 0;
}
