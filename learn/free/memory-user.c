#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE (900 * 1000 * 1000) // 900 Mb

int main(int argc, char **argv)
{
    char * ptr = malloc (SIZE);
    int i = 0;
    while (1) {
       memset(ptr, i, SIZE);
       i++;
    }
    return 0;
}
