#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    /*
     * allocate memory and assign address to p.
     * Also, take a local variable in stack.
     */
    int * p;
    int q = 15;
   
   
    p = (int *) malloc (sizeof(int));
    *p = 10;
    printf("\n Value stored at heap memory is : %d\n", *p);

    /*
     * Now, assign the address of q (local variable in stack), to p, without 
     * freeing the earlier allocated memory
     *
     * We are trying to leak the memory allocated in heap.
     */

    p = &q;
    printf("\n Value stored at stack memory is : %d\n", *p);

    return 0;

}
