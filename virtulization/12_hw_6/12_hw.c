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
        printf("Hello, I am child(pid : %d). I will call execv to launch ls "
                "command\n", getpid());
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

        printf("Hello, I am parent (pid : %d). waiting for child : %d to finish"
                ".\n", getpid(), rc);
        waitpid(rc, NULL, 0);
        printf("I am parent, executing after child pid(pid : %d) has finished"
                "\n",
                rc);
    }
    return 0;
}
