#include <stdio.h>
/*
 * File system implementation.
 */

#define DISK_START 0x00000000
#define BLOCK_SIZE 0x1000 // 4096 bytes

/*
 * Super block.
 * One block is sufficient to store file system related data.
 */
#define SUPER_BLOCK_START_OFFSET DISK_START
#define SUPER_BLOCK_SIZE (1 * BLOCK_SIZE)

/*
 * Inodes free list
 *
 */
#define INODE_FREE_LIST_START_OFFSET \
    (SUPER_BLOCK_START_OFFSET + SUPER_BLOCK_SIZE)
#define INODE_FREE_LIST_SIZE (1 * BLOCK_SIZE)

/*
 * Data blocks free list
 */
#define DATA_FREE_LIST_START_OFFSET \
    (INODE_FREE_LIST_START_OFFSET + INODE_FREE_LIST_SIZE)
#define DATA_FREE_LIST_SIZE (8 * BLOCK_SIZE)

/*
 * I-nodes array
 */
#define INODES_ARRAY_START_OFFSET \
    (DATA_FREE_LIST_START_OFFSET + DATA_FREE_LIST_SIZE)
#define INODES_ARRAY_SIZE (40 * BLOCK_SIZE)

/*
 * Data blocks
 */
#define DATA_BLOCKS_START_OFFSET \
    (INODES_ARRAY_START_OFFSET + INODES_ARRAY_SIZE)
#define DATA_BLOCKS_SIZE 0x40000 // In bytes
