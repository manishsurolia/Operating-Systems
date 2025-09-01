#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child process (PID=%d) exiting...\n", getpid());
        exit(0);
    } else {
        printf("Parent process (PID=%d) sleeping...\n", getpid());
        sleep(30);  // Chlid quickly exited it has gone in the defunct state.
        wait(NULL);  // Now, the child will come out of the defunct state.
        printf("Parent exiting...\n");
    }

    return 0;
}

