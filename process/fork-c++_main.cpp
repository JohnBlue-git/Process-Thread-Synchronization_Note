#include <iostream>     // For cout << , will also include <stdlib.h>
#include <unistd.h>     // For fork, getpid, and waitpid
#include <sys/wait.h>   // For waitpid
#include <sys/types.h>  // For pid_t
#include <vector>       // For vector

void process_context() {
    // ...
}

int main() {
    // child processes container
    std::vector<pid_t> childProcesses;

    // Fork process
    for (int i = 0; i < 3; ++i) {
        // fork process
        pid_t pid = fork();

        if (pid == 0) { // Child process
            std::cout << "Child!" << std::endl;
            process_context();
            return 0;
        } else if (pid > 0) { // Parent process
            std::cout << "Parent!" << std::endl;
            childProcesses.push_back(pid);
        } else { // pid < 0  Fork failed
            std::cerr << "Fork failed!" << std::endl;
            return 1;
        }
        // note: getpid() can also get current pid
    }

    // Wait for all child processes to finish
    for (pid_t pid : childProcesses) {
        waitpid(pid, NULL, 0);
    }

    // 0 EXIT_SUCCESS
    // 1 EXIT_FAILURE
    
    // return will clear, exit, and return value.
    // exit() is returned to the operating system as the program's return code or exit code. By convention, a return code of zero means that the program completed successfully. You can use the constants EXIT_FAILURE and EXIT_SUCCESS.
    // abort() terminates the program immediately. The abort function bypasses the normal destruction process for initialized global static objects. It also bypasses any special processing that was specified using the atexit function.
    return 0;
}
