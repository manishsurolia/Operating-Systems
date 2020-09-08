#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#define TOTAL_SYS_CALLS 100000000
#define TOTAL_SYS_CALLS1 10000000
#define MICROSECONDS_IN_SECOND 1000000
#define BUFF_SIZE 100

typedef unsigned long ulong;

void time_taken_by_one_sys_call()
{
    int fd;
    struct timeval time;
    ulong time_before_system_calls;
    ulong time_after_system_calls;
    char buffer[100];
    if(unlink("./temp.txt")) {
        fprintf(stderr,"unlink failed. %s\n",strerror(errno));
        exit(1);
    }
    fd = open("./temp.txt", O_CREAT|O_RDONLY|O_EXCL);
    if(fd == -1) {
        fprintf(stderr, "Open() failed. error: %s\n", strerror(errno));
        exit(1);
    } else {

        if(!gettimeofday(&time, NULL)) {
            time_before_system_calls = time.tv_sec;
        } else {
            fprintf(stderr, "gettimeofday() failed. Error : %s\n",
                    strerror(errno));
        }

        for(ulong count = 0; count <TOTAL_SYS_CALLS; count++) {
            if(read(fd, buffer, 100) == -1) {
                fprintf(stderr, "read() failed. Error : %s\n",
                        strerror(errno)); 
                exit(1);
            }
        }

        if(!gettimeofday(&time, NULL)) {
            time_after_system_calls = time.tv_sec;
        } else {
            fprintf(stderr,"gettimeofday() failed. Error : %s\n",
                    strerror(errno));
        }

        ulong time_taken_in_secs = time_after_system_calls - 
            time_before_system_calls;

        printf("Time taken for 1 system call : %.2f µs\n",
               (float)((ulong)(time_taken_in_secs * MICROSECONDS_IN_SECOND)) /
               TOTAL_SYS_CALLS);
    }
    return;
}


void time_taken_by_one_context_switch()
{
    int rc;
    int fd1[2];
    int fd2[2];
    char buff[BUFF_SIZE];
    struct timeval time;
    ulong time_before_system_calls;
    ulong time_after_system_calls;

    rc = pipe(fd1);
    if(rc == -1) {
        fprintf(stderr, "pipe() failed. Error : %s\n",strerror(errno));
        exit(1);
    }
    rc = pipe(fd2);
    if(rc == -1) {
        fprintf(stderr, "pipe() failed. Error : %s\n",strerror(errno));
        exit(1);
    }
    rc = fork();
    if(rc < 0) {
        fprintf(stderr, "fork() failed. Error : %s\n", strerror(errno));
    } else if (rc == 0) {
        /*
         * In child process, close read for fd1, and write for fd2.
         */
        close(fd1[0]);
        close(fd2[1]);

        /*
         * write to the fd1 pipe and read from fd2.
         */
        for(ulong counter = 0; counter < TOTAL_SYS_CALLS1; counter++) {
            if(write(fd1[1], "C", 2)
                        == -1) {
                fprintf(stderr,"write from child process failed. Error : %s\n",
                        strerror(errno));
            } else {
                if(read(fd2[0], buff, BUFF_SIZE) == -1) {
                    fprintf(stderr, "read() failed. Error : %s\n",
                            strerror(errno));
                    exit(1);
                }
            }
        }
    } else {
        /*
         * In parent process, close read for fd1, and write for fd2.
         */
        close(fd1[1]);
        close(fd2[0]);

        if(!gettimeofday(&time, NULL)) {
            time_before_system_calls = time.tv_sec;
        } else {
            fprintf(stderr, "gettimeofday() failed. Error : %s",
                    strerror(errno));
            exit(1);
        }

        /*
         * write to the fd2 pipe and read from fd1.
         */
        for(ulong counter = 0; counter < TOTAL_SYS_CALLS1; counter++) {
            if(write(fd2[1], "P", 2) == -1) {
                fprintf(stderr,"write() from parent process failed. "
                        "Error : %s\n", strerror(errno));
                exit(1);
            } else {
                if(read(fd1[0], buff, BUFF_SIZE) == -1) {
                    fprintf(stderr, "read() failed. Error : %s\n",
                            strerror(errno));
                    exit(1);
                }
            }
        }

        if(!gettimeofday(&time, NULL)) {
            time_after_system_calls = time.tv_sec;
        } else {
            fprintf(stderr, "gettimeofday() failed. Error : %s",
                    strerror(errno));
            exit(1);
        }

        ulong time_taken_in_secs = time_after_system_calls -
            time_before_system_calls;

        printf("Time taken for 1 context switch : %.2f µs\n",
                (float)((ulong)(time_taken_in_secs * MICROSECONDS_IN_SECOND)) /
                TOTAL_SYS_CALLS1);
    }
}

int main(int argc, char *argv[])
{
    time_taken_by_one_sys_call();    
    time_taken_by_one_context_switch();
    return 0;
}
