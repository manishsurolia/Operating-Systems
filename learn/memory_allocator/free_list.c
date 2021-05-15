/*
 * This free-list implementation is only for maximum of 4KB of memory.
 */
#include "free_list.h"

/*
 * Global data to hold the head of free list.
 */
static node_t *head;

static node_t * get_free_list (void)
{
    return head;
}

static void update_free_list (node_t *list)
{
    head = list;
}

static void * get_heap_from_kernel(void) {
    static int count;
    node_t *heap = NULL;
    if (!count) { // For now, the request will be served only once for 4KB.
        heap = mmap(NULL, HEAP_SIZE, (PROT_READ|PROT_WRITE),
                (MAP_ANON | MAP_PRIVATE), -1, 0);
        if (heap == MAP_FAILED) {
            perror(strerror(errno));
            exit (1);
        } else {
            heap->size = HEAP_SIZE;
            heap->next = NULL;
        }
        count++;
    }
    return heap;
}

/*
 * 'malloc' never creates a new node in the free list.
 * It just parse through the list of free nodes and try to find requested chunk
 * of memory.
 * Once sufficient memory is found in a node, it shifts the node header to
 * either by the requested size or to the next node.
 */
static node_t * malloc_internal(node_t *p, int requested_size, void **output,
                         int *allocated_size)
{
    int available_size;
    int remaining_size;
    node_t *next;

    if (!p) {
        *output = NULL;
        *allocated_size = 0;
    } else {
        available_size = p->size;
        if (available_size >= requested_size) {
            *output = p;
            remaining_size = available_size - requested_size;
            if (remaining_size > sizeof(node_t)) {
                *allocated_size = requested_size;
                next = p->next;
                p = (void *)p + (*allocated_size);
                p->size = remaining_size;
                p->next = next;
            } else {
                *allocated_size = available_size;
                p = p->next;
            }
        } else {
            p->next = malloc_internal(p->next, requested_size, output,
                      allocated_size);
        }
    }

    return p;
}

void * my_malloc (int size)
{
    node_t *list;
    void * ret = NULL;
    int allocated_size;
    int requested_size;

    requested_size  = sizeof(header_t) + size;
    list = get_free_list();

    if (!list) {
        list = get_heap_from_kernel();
    }

    if (list) {
        list = malloc_internal(list, requested_size, &ret, &allocated_size);
        if (!ret) {
            // Didn't get the requested memory. Request more memory from OS.
            // TBD later
        } else {
            ((header_t *)ret)->magic = MAGIC_HEADER;
            ((header_t *)ret)->size = allocated_size;
            ret = ret + sizeof(header_t);
        }
    }

    update_free_list(list);
    return ret;
}

int main(int argc, char **argv)
{
    while(1) {
        if (!my_malloc(100)) {
            break;
        } else {
            printf("100 bytes allocted.\n");
        }
    }

    while(1) {
        if (!my_malloc(50)) {
            break;
        } else {
            printf("50 bytes allocted.\n");
        }
    }

    while(1) {
        if (!my_malloc(1)) {
            break;
        } else {
            printf("1 byte allocted.\n");
        }
    }

    return 0;
}
