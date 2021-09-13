/*
 * This program shows how to share memory using mmap system call.
 * This program creates a child process, and both the parent and child share
 * memory which was created using mmap.
 *
 * Note : mmap can be used to share a memory only between 2 related processes.
 * like child and parent process.
 */

#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdint.h>
#include <sys/mman.h>

#define PAGESIZE 4096

int unshared_memory = 5;

int main(int argc, int **argv)
{
    uint8_t *shared_memory = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *shared_memory = 34;
    if (fork() == 0) {
        /*
         * child process.
         * Making change to both shared and non-shared memory.
         */
        *shared_memory = 15;
        unshared_memory = 80;
    } else {
        wait(NULL);
    }

    printf("Process id : %d\n",getpid());
    printf("Not shared. %i\n", unshared_memory);
    printf("Shared. %i\n\n\n", *shared_memory);
    return 0;
}
