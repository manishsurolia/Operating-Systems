#include <stdio.h>
#include <sys/syscall.h>

int main(void)
{
    for (unsigned int i=0; i<10000000; i++) {
        syscall(SYS_time, NULL);
    }
    return 0;
}
