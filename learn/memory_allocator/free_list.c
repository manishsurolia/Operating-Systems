#include "free_list.h"

void * get_heap(void) {
    node_t *heap = NULL;
    heap = mmap(NULL, HEAP_SIZE, (PROT_READ|PROT_WRITE),
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
    static node_t *head;
    static int allocated; /* Only one time allocation */
    void * ret;
    int requested_size = sizeof(header_t) + size;

    if (!allocated) {
        head = get_heap();
        allocated = 1;
    }

    /* First fit Implementation */ 
    while (head) {
        int available_chunk_size = sizeof(node_t) + head->size;
        if (requested_size < available_chunk_size) {

            ret = head;
            ((header_t *)ret)->magic = MAGIC_HEADER;
            ((header_t *)ret)->size = size;

            if ((available_chunk_size - requested_size) >= sizeof(node_t)) {
                head = (void *)head + requested_size;
                head->size = available_chunk_size - requested_size -
                             sizeof(node_t);
                head->next = NULL;
            } else {
                head = NULL;
            }
            return (ret + sizeof(header_t));
        }

        if (head->next) {
            head = head->next;
        } else {
            break;
        }
    }
    return  NULL;
}

int main(int argc, char **argv)
{
    void *p;
    while(1) {
        if (!my_malloc(1000)) {
            printf("Got NULL, no more sufficient memory available in heap\n");
            break;
        } else {
            printf("1000 bytes allocted.\n");
        }

    }

    while(1) {
        if (!my_malloc(50)) {
            printf("Got NULL, no more sufficient memory available in heap\n");
            break;
        } else {
            printf("50 bytes allocted.\n");
        }

    }

    while(1) {
        if (!my_malloc(1)) {
            printf("Got NULL, no more sufficient memory available in heap\n");
            break;
        } else {
            printf("1 byte allocted.\n");
        }

    }


    return 0;
}
