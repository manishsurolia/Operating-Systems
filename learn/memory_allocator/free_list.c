#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

#define HEAP_SIZE 4096
#define MAGIC_HEADER 0x12ABCD34

typedef struct __header_t {
    int magic;
    int size;
} header_t;

typedef struct __node_t {
    int size;
    struct __node_t *next;
} node_t;

void * get_heap(void) {
    node_t *heap = mmap(NULL, HEAP_SIZE, (PROT_READ|PROT_WRITE),
                  (MAP_ANON | MAP_PRIVATE), -1, 0);
    if (heap == MAP_FAILED) {
        perror(strerror(errno));
        exit (1);
    } else {
        heap->size = HEAP_SIZE - sizeof(node_t); 
        heap->next = NULL;
    }
    return heap;
}

void * my_malloc(int size)
{
    static node_t *heap;
    void * ret;

    if (!heap) {
        heap = get_heap();
    }

    /* First fit Implementation */ 
    while (heap) {
        if ((sizeof(node_t) + heap->size) >= (sizeof(header_t) + size)) {
            ret = heap + sizeof(node_t);  
            ((header_t *)ret)->magic = MAGIC_HEADER;
            ((header_t *)ret)->size = size;
            heap->next = ret + sizeof(header_t) + size;
            return (ret + sizeof(header_t));
        }
        heap = heap->next;
    }
    return  NULL;
}

int main(int argc, char **argv)
{
    void *p;
    while(1) {
        p = my_malloc(1000); 
        if (!p) {
            break;
        } else {
            printf("1000 bytes allocted.\n");
        }
    }
    return 0;
}
