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

    fsync(fd);
    return;
}

int main(void)
{
    fs_format();
    return 0;
}
