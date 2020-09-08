#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int rc;
    char * args[3];
    rc = fork();

    if(rc < 0) {
        fprintf(stderr, "fork() failed");
        exit(1);
    } else if (rc == 0) {
        printf("Hello, I am child. I will call execv to launch ls command\n");
        args[0] = "/bin/ls";
        args[1] = "-lart";
        args[2] = NULL;
        if(execv("/bin/ls", args) == -1) {
            printf("execv returned. That means, some problem occured.\n");
        }
    } else {
        /*
         * waiting for child process to complete
         */
        wait(NULL);
        printf("Hello, I am parent.\n");
    }
    return 0;
}
