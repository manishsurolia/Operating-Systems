#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    int rc;
    rc = fork();
    if(rc < 0) {
        fprintf(stderr,"fork() failed\n");
        exit(1);
    } else if(rc ==0) {
        printf("Hello, I am child process. I will close STDOUT_FILENO and try"
                " to use printf");
        close(STDOUT_FILENO);
        printf("Trying to print string after closing STDOUT_FILENO file "
                "descriptor\n");
    } else {
        waitpid(rc, NULL, 0);
        printf("Hello, I am parent process. See child has closed the default"
                " output file descriptor(STDOUT_FILENO), now its printf is "
                "not working\n");
    }
    return 0;
}

