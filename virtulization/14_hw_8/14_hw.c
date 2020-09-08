#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int rc;
    int fd[2];
    printf("Parent process(pid : %d)  I will create a pipe and will use"
            " it in parent and child processes\n", getpid());
    if(pipe(fd) == -1) {
        fprintf(stderr,"pipe() failed.\n");
        exit(1);
    } else {
        rc = fork();
        if(rc < 0) {
            fprintf(stderr, "fork() failed.\n");
            exit(1);
        } else if( rc == 0) {
            printf("Child process(pid : %d). I will use fd[1] to write to"
                    " the pipe.\n",getpid());
            close(fd[0]);
            //write(fd[1], "MESSAGE_FROM_CHILD", (strlen("MESSAGE_FROM_CHILD")+1));
            /*
             * instead of directly writing with 'write', I am making fd[1] as 
             * default output. So, that even 'printf()' can be used as usual.
             */
            dup2(fd[1], 1);
            printf("MESSAGE_FROM_CHILD");
        } else {
            waitpid(rc, NULL, 0);
            char buff[100];
            printf("Parent process(pid : %d). I will use fd[0] to read from"
                    " the pipe.\n",getpid());
            close(fd[1]);
            //read(fd[0], buff, 100);
            /*
             * instead of directly writing with 'write', I am making fd[0] as 
             * default input. So, that even 'scanf()' can be used as usual.
             */
            dup2(fd[0], 0);
            scanf("%s",buff);
            printf("Message got from child process : %s\n",buff);
        }
    }
    return 0;
}
