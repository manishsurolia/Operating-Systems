#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    int *data = NULL;
    data = (int *) malloc(sizeof(int) * 100);

    /* 
     * The above malloc has allocated memory for 100 integers.
     * We can asssume it as an array from 0 - 99 indexes.
     */

    /* 
     * Accessing out of boundary memory
     */
    data[100] = 0; 
    return 0;

}
