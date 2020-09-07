#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int x = 100;
    int rc = 0;
    int counter = 0;

    rc = fork();

    if(rc < 0) {
        fprintf(stderr, "fork() failed.\n");
        exit(1);
    } else if(rc == 0) {
        printf("Hello, I am child (pid : %d) process. I will keep incrementing " 
                "%d with one\n",
                getpid(), x);
        for(counter=0; counter <25; counter++) {
            sleep(2);
            x = x + 1;
            printf("(pid : %d), x = %d\n",getpid(), x); 
        }
    } else {
        printf("Hello, I am parent (pid : %d). I will keep incrementing %d with" 
                " 2\n", getpid(), x);
        for(counter=0; counter <25; counter++) {
            sleep(1);
            x = x + 2;
            printf("(pid : %d), x = %d\n",getpid(), x); 
        }
    }
    return 0;
}
