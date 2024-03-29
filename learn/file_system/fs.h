#include <stdio.h>
/*
 * File system implementation.
 */

/*
 * All sizes are in bytes, by default.
 */
#define DISK_START 0x00000000
#define BLOCK_SIZE 4096
#define INODE_SIZE 256
#define INODE_BLOCKS 64
#define INODE_MAX ((INODE_BLOCKS * BLOCK_SIZE) / INODE_SIZE)
#define INODE_MAGIC 0xAB1234EF
#define SUPER_BLOCK_MAGIC 0x12ABCD34
#define FILE_NAME_LEN 32
#define BITS_PER_BYTE 8

#define FILE_ERR -1
#define FILE_CLOSED 0
#define FILE_OPENED 1
#define FILE_NOT_FOUND -1
#define NO_FREE_INODE -1
/*
 * Super block.
 * One block is sufficient to store file system related data.
 */
#define SUPER_BLOCK_START DISK_START
#define SUPER_BLOCK_SIZE (1 * BLOCK_SIZE)

/*
 * Inodes free list
 *
 */
#define INODE_FREE_LIST_START_OFFSET \
    (SUPER_BLOCK_START + SUPER_BLOCK_SIZE)
#define INODE_FREE_LIST_SIZE (1 * BLOCK_SIZE)

/*
 * Data blocks free list
 */
#define DATA_FREE_LIST_START_OFFSET \
    (INODE_FREE_LIST_START_OFFSET + INODE_FREE_LIST_SIZE)
#define DATA_FREE_LIST_SIZE (8 * BLOCK_SIZE)

/*
 * I-node array. As of now we support only 1024 files in our file system.
 * Hence, 64 blocks (each of size 4096 bytes) are enough for 256 bytes inode.
 */
#define INODES_ARRAY_START_OFFSET \
    (DATA_FREE_LIST_START_OFFSET + DATA_FREE_LIST_SIZE)
#define INODES_ARRAY_SIZE (0x40 * BLOCK_SIZE)

/*
 * Data blocks
 */
#define DATA_BLOCKS_START_OFFSET \
    (INODES_ARRAY_START_OFFSET + INODES_ARRAY_SIZE)
#define DATA_BLOCKS_SIZE 0x40000 // In bytes

/*
 * Bit operations on an array of char. The passed array must be a char array.
 * Marking it as int or long array will involve endianness issues.
 * With just char array, below 3 bitwise operation macros should work fine with
 * any endian(little or big) machine.
 * TBD: show pictorically, how we are using bits in array of char
 */
#define SET_BIT_IN_ARRAY(arr, bit) \
    arr[bit / (sizeof(*arr) * BITS_PER_BYTE)] |= \
    ((0x80) >> (bit % (sizeof(*arr) * BITS_PER_BYTE)));

#define CLEAR_BIT_IN_ARRAY(arr, bit) \
    arr[bit / (sizeof(*arr) * BITS_PER_BYTE)] &= \
    ~((0x80) >> (bit % (sizeof(*arr) * BITS_PER_BYTE)));

#define TEST_BIT_IN_ARRAY(arr, bit) \
    arr[bit / (sizeof(*arr) * BITS_PER_BYTE)] & \
    ((0x80) >> (bit % (sizeof(*arr) * BITS_PER_BYTE)))

typedef struct inode {
    union {
        struct {
            unsigned int magic; // inode magic number.
            unsigned char file_name[FILE_NAME_LEN];
            unsigned int status; // OPEN or CLOSED
            unsigned long long file_size; // file size in bytes
        }data;
        char buffer[INODE_SIZE]; // Keep each inode size as 'INODE_SIZE'.
    };
}inode;

typedef struct super_block {
    union {
        struct {
            unsigned int magic; // Super block magic number
            void *inode_free_list_start_address;
            void *data_free_list_start_address;
            void *inode_arr_start_address;
            void *data_blocks_start_adddress;
        }data;
        char buffer[BLOCK_SIZE]; // Taking a whole block as super block
    };
}super_block;
