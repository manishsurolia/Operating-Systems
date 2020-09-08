#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd;
    int rc;

    fd = open("./output.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    rc = fork();
    if(rc < 0) {
        fprintf(stderr, "fork() failed");
        exit(1);
    } else if(rc == 0) {
        printf("Hello, I am child (pid : %d), writing 'CHILD' in the file.\n", 
                getpid());
        while(1) {
            sleep(2);
            write(fd, "CHILD\n", 6);
        }
    } else {
        /*
         * NO waiting for child to finish, but adding some delay of 1 sec.
         */
        printf("Hello, I am parent (pid : %d) of %d, writing 'PARENT' in the "
                "file.\n", getpid(), rc);
        while(1) {
            sleep(1);
            write(fd, "PARENT\n", 7);
        }
    }
    return 0;
}
