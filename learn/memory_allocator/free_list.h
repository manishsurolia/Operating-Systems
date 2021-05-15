#ifndef FREE_LIST_H
#define FREE_LIST_H

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
    int magic;
    int size;
    struct __node_t *next;
} node_t;

#endif /* FREE_LIST_H */
