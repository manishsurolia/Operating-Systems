#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int rc;
    rc = fork();
    if(rc < 0) {
        fprintf(stderr, "fork() failed.\n");
        exit(1);
    } else if(rc == 0) {
        printf("Hello, I am child process. (pid : %d)\n", getpid());
    } else {
        sleep(1);
        printf("Goodbye, I am parent process. (pid: %d)\n",getpid());
    }
    return 0;
}
