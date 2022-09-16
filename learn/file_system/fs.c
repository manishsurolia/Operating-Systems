#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
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

    ((super_block *)base_address)->data.magic = SUPER_BLOCK_MAGIC;
    msync(base_address, BYTES_IN_1GB, MS_SYNC|MS_INVALIDATE);
    munmap(base_address, BYTES_IN_1GB);
    close(fd);
    return;
}

void *fs_mount(int *fd)
{
    void *base_address;

    if (!fd) {
        printf("%s. Passed fd pointer is NULL.\n", __func__);
        return NULL;
    }

    /* Create a linux file descriptor with a file */
    *fd = open(LOCAL_FILE, O_RDWR, S_IRUSR|S_IWUSR);

    /* mmap the file in the address space of this program. */
    base_address = mmap(NULL, BYTES_IN_1GB, PROT_READ | PROT_WRITE,
                            MAP_SHARED, *fd, 0);

    if (base_address == MAP_FAILED) {
        return NULL;
    }

    /*
     * Initialize the super block pointers.
     */
    if (((super_block *)base_address)->data.magic == SUPER_BLOCK_MAGIC) {
        ((super_block *)base_address)->data.inode_free_list_start_address =
            ((char *)base_address + INODE_FREE_LIST_START_OFFSET);
        ((super_block *)base_address)->data.data_free_list_start_address =
            ((char *)base_address + DATA_FREE_LIST_START_OFFSET);
        ((super_block *)base_address)->data.inode_arr_start_address =
            ((char *)base_address + INODES_ARRAY_START_OFFSET);
        ((super_block *)base_address)->data.data_blocks_start_adddress =
            ((char *)base_address + DATA_BLOCKS_START_OFFSET);
    }

    return base_address;
}

/*
 * Return -1 if fails, otherwise return offset of inode free list bitmap.
 */
int fs_create_file(void *fs_base_address, char *name)
{
    super_block *sb = fs_base_address;
    int free_inode_bit = NO_FREE_INODE;
    unsigned int *free_inode_bit_array;
    struct inode *inode_table;

    if (!sb || !name) {
        printf("%s : Wrong values passed.\n", __func__);
        return -1;
    }

    if (strlen(name) >= (FILE_NAME_LEN-1)) {
        printf("%s : passed file name exceeds the premitted lenght of %s"
               " bytes\n", FILE_NAME_LEN);
        return -1;
    }

    free_inode_bit_array = sb->data.inode_free_list_start_address;
    inode_table = sb->data.inode_arr_start_address;

    // Find the free bit in inode free list.
    for (unsigned int i=0; i<INODE_MAX; i++) {
        if (TEST_BIT_IN_ARRAY(free_inode_bit_array, i)) {
            if (!strcmp(inode_table[i].data.file_name, name)) {
                return i;
            }
        }
    }


    // Find the free bit in inode free list.
    for (unsigned int i=0; i<INODE_MAX; i++) {
        if (!TEST_BIT_IN_ARRAY(free_inode_bit_array, i)) {
            free_inode_bit = i;
            break;
        }
    }

    /*
     * Inodes should be in the range of 0 to 'INODE_MAX-1'.
     */
    if (free_inode_bit == NO_FREE_INODE) {
        printf("No free inode in the file system.\n");
        return -1;
    }

    // Populate the inode entry in the inode_table.
    inode_table[free_inode_bit].data.magic = INODE_MAGIC;
    inode_table[free_inode_bit].data.file_size = 0;
    inode_table[free_inode_bit].data.status = FILE_CLOSED;
    strcpy(inode_table[free_inode_bit].data.file_name, name);

    // Finally, Mark the inode bit TRUE in inode free list, to represent that
    // this inode is consumed by a file.
    SET_BIT_IN_ARRAY(free_inode_bit_array, free_inode_bit);

    msync(fs_base_address, BYTES_IN_1GB, MS_SYNC|MS_INVALIDATE);

    // Return the inode bit which got updated.
    return free_inode_bit;
}

int main(void)
{
    void *fs_base_address;
    int fd = 0;
    fs_format();
    printf("format complete.\n");
    fs_base_address = fs_mount(&fd);
    if (!fs_base_address) {
        printf("No file system present.\n");
    }
    fs_create_file(fs_base_address, "file1");
    fs_create_file(fs_base_address, "file2");
    fs_create_file(fs_base_address, "file3");
    return 0;
}
