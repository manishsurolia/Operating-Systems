#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int *data = (int *) malloc (sizeof(int) * 100);

    data[0] = 10;
    printf("\nValue at index 0 (before free) is data[0]: %d\n", data[0]);

    free(data);
    printf("\nValue at index 0 (after free) is data[0]: %d\n", data[0]);

    return 0;

}
