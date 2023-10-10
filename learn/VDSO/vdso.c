#include <stdio.h>
#include <time.h>

int main(void)
{
    for (unsigned int i=0; i<10000000; i++) {
        time(NULL);
    }
    return 0;
}
