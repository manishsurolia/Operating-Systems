#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include "fs.h"

#define LOCAL_FILE "./file"
#define BYTES_IN_1GB 1*1024*1024*1024

void fs_format(void)
{
    void *base_address;
    char *buf;

    /* Create a linux file descriptor with a file */
    int fd = open(LOCAL_FILE, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    /* mmap the file in the address space of this program. */
    base_address = mmap(NULL, BYTES_IN_1GB, PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);

    /* Reset the whole file disk, before using it. */
    buf = (char *) malloc (BYTES_IN_1GB);
    memset(buf, 0, BYTES_IN_1GB);
    write(fd, buf, BYTES_IN_1GB);
    free(buf);

    /* Initialize the super block */
    ((super_block *)base_address)->data.magic = SUPER_BLOCK_MAGIC;
    ((super_block *)base_address)->data.inode_free_list_start_address =
        base_address + INODE_FREE_LIST_START_OFFSET;
    ((super_block *)base_address)->data.data_free_list_start_address =
        base_address + DATA_FREE_LIST_START_OFFSET;
    ((super_block *)base_address)->data.inode_arr_start_address =
        base_address + INODES_ARRAY_START_OFFSET;
    ((super_block *)base_address)->data.data_blocks_start_adddress =
        base_address + DATA_BLOCKS_START_OFFSET;

    /* Initialize the next free inode and next free data block */
    ((super_block *)base_address)->data.free_inode_bit = 0;
    ((super_block *)base_address)->data.free_data_block_bit = 0;

    msync(base_address, BYTES_IN_1GB, MS_SYNC|MS_INVALIDATE);
    munmap(base_address, BYTES_IN_1GB);
    return;
}

void *fs_mount(void)
{
    void *base_address;
    char *buf;

    /* Create a linux file descriptor with a file */
    int fd = open(LOCAL_FILE, O_RDWR, S_IRUSR|S_IWUSR);

    /* mmap the file in the address space of this program. */
    base_address = mmap(NULL, BYTES_IN_1GB, PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);

    if (base_address == MAP_FAILED) {
        return NULL;
    }
    return base_address;
}
/*
 * Return -1 if fails, otherwise return offset of inode free list bitmap.
 */
int fs_create_file(void *fs_base_address, char *name)
{
    super_block *sb = fs_base_address;
    unsigned int free_inode_bit;
    unsigned int *free_inode_bit_array = sb->data.inode_arr_start_address;
    if (!sb || !name) {
        printf("%s : Wrong values passed.\n", __func__);
        return -1;
    }
    free_inode_bit = sb->data.free_inode_bit;

    /*
     * Inodes should be in the range of 0 to 'INODE_MAX-1'.
     */
    if (free_inode_bit >= INODE_MAX) {
        printf("Invalid free_inode_bit.\n");
        return -1;
    }

    // Mark the inode bit TRUE in inode free list, to represent that this inode
    // is consumed by a file.
    free_inode_bit_array[free_inode_bit/sizeof(unsigned int)] |=
        1 << (free_inode_bit%sizeof(unsigned int));

    // Populate the inode with file name.

    // Return the bit which got updated.
    return free_inode_bit;
}

void fs_delete_file(char *name);
void fs_list_files(char *name);

int main(void)
{
    void *fs_base_address;
    fs_format();
    printf("format complete.\n");
    fs_base_address = fs_mount();
    if (!fs_base_address) {
        printf("No file system present.\n");
    }
    fs_create_file(fs_base_address, "file1");
    getchar();
    return 0;
}
