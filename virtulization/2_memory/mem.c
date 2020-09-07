#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int *p = NULL;
    int init_value = 0;
    if(argc != 2) {
        fprintf(stderr, "usage : ./mem <number>\n");
        exit(1);
    }
    p = malloc(sizeof(int));
    if (p == NULL) {
        printf("malloc failed");
    } 
    printf("\nMemory (%d) address of p: %p\n",getpid(), p);

    init_value = atoi(argv[1]);
    *p = init_value;
    while(1) {
        sleep(1);
        *p = *p + 1;
        printf("(%d) p: %d\n",getpid(), *p);
    }
    return 0;
}
