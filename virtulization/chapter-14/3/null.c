#include <stdio.h>

int main(int argc, char *argv[])
{
    int * p = NULL;
    int q;

    /* 
     * Trying to dereference a NULL address.
     */
    q = *p;
    return 0;
}
