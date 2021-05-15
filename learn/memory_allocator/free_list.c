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

static void print_free_list_internal (node_t *list)
{
    if (list) {
        printf("Address : %p, Size : %d, next : %p\n", list, list->size,
                list->next);
        print_free_list_internal(list->next);
    }
}

void print_free_list (void)
{
    print_free_list_internal(get_free_list());
}

static unsigned long total_free_space_internal (node_t *list)
{
    if (!list) {
        return 0;
    }

    return (list->size + total_free_space_internal(list->next));
}


void print_total_free_space (void)
{
    printf("Total free space : %lu\n",
            total_free_space_internal(get_free_list()));
}

static void * get_heap_from_kernel (void) {
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
static node_t * malloc_internal (node_t *p, int requested_size, void **output,
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

void my_free (void *p)
{
    int size;
    if (!p) {
        return;
    }

    p = p - sizeof(header_t);
    size = ((header_t *)p)->size;

    if (((header_t *)p)->magic == MAGIC_HEADER) {
        ((node_t *)p)->size = size;
        ((node_t *)p)->next = get_free_list();
        update_free_list(p);
    } else {
        exit (1); // Heap corrupted. Terminate the process.
    }
}


int main (int argc, char **argv)
{
    void *ptr;

    printf("===========================================================\n");
    if (ptr = my_malloc(100)) {
        printf("100 bytes allocated.\n");
        my_free(ptr);
        printf("100 bytes freed.\n");
        print_free_list();
        print_total_free_space();
    } else {
        printf("100 bytes allocation failed.\n");
    }
    printf("===========================================================\n");

    if (ptr = my_malloc(200)) {
        printf("200 bytes allocated.\n");
        my_free(ptr);
        printf("200 bytes freed.\n");
        print_free_list();
        print_total_free_space();
    } else {
        printf("200 bytes allocation failed.\n");
    }
    printf("===========================================================\n");

    if (ptr = my_malloc(500)) {
        printf("500 bytes allocated.\n");
        my_free(ptr);
        printf("500 bytes freed.\n");
        print_free_list();
        print_total_free_space();
    } else {
        printf("500 bytes allocation failed.\n");
    }
    printf("===========================================================\n");

    if (ptr = my_malloc(1000)) {
        printf("1000 bytes allocated.\n");
        my_free(ptr);
        printf("1000 bytes freed.\n");
        print_free_list();
        print_total_free_space();
    } else {
        printf("1000 bytes allocation failed.\n");
    }
    printf("===========================================================\n");

    if (ptr = my_malloc(50)) {
        printf("50 bytes allocated.\n");
        my_free(ptr);
        printf("50 bytes freed.\n");
        print_free_list();
        print_total_free_space();
    } else {
        printf("50 bytes allocation failed.\n");
    }
    printf("===========================================================\n");

    if (ptr = my_malloc(1000)) {
        printf("1000 bytes allocated.\n");
        my_free(ptr);
        printf("1000 bytes freed.\n");
        print_free_list();
        print_total_free_space();
    } else {
        printf("1000 bytes allocation failed.\n");
    }
    printf("===========================================================\n");

    if (ptr = my_malloc(2000)) {
        printf("2000 bytes allocated.\n");
        my_free(ptr);
        printf("2000 bytes freed.\n");
        print_free_list();
        print_total_free_space();
    } else {
        printf("2000 bytes allocation failed.\n");
    }
    printf("===========================================================\n");
    return 0;
}
