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

/*
 * Base address of the file system.
 */
static void *fs_base_address;

/*
 * Erase all data in the disk and mark first block as super block.
 */
void format (void)
{
    void *base_address;
    char *buf;

    /*
     * Create a linux file descriptor with a file.
     */
    int fd = open(LOCAL_FILE, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    /*
     * mmap the file in the address space of this program.
     */
    base_address = mmap(NULL, BYTES_IN_1GB, PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);

    /*
     * Reset the whole file disk, before using it.
     */
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

/*
 * Mount already existing disk.
 */
void *mount (int *fd)
{
    void *base_address;
    struct inode *inode_table;

    if (!fd) {
        printf("%s. Passed fd pointer is NULL.\n", __func__);
        return NULL;
    }

    /*
     * Create a linux file descriptor with a file.
     */
    *fd = open(LOCAL_FILE, O_RDWR, S_IRUSR|S_IWUSR);

    /*
     * mmap the file in the address space of this program.
     */
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

    /*
     * Wihle mounting a disk, no file is opened, so mark each file as closed.
     * Important to recover from last incorrect unmounting of the disk.
     */
    inode_table = ((super_block *)base_address)->data.inode_arr_start_address;
    for (int i=0; i<INODE_MAX; i++) {
        inode_table[i].data.status = FILE_CLOSED;
    }

    return base_address;
}

/*
 * Un-mount already existing disk.
 */
void unmount (int fd)
{
    munmap(fs_base_address, BYTES_IN_1GB);
    close(fd);
    return;
}

/*
 * Return 'FILE_ERR' if fails, otherwise return offset of inode free list
 * bitmap.
 */
int create (char *name)
{
    super_block *sb = fs_base_address;
    int free_inode_bit = NO_FREE_INODE;
    unsigned char *free_inode_bit_array;
    struct inode *inode_table;

    if (!sb || !name) {
        printf("%s : Wrong values passed.\n", __func__);
        return FILE_ERR;
    }

    if (strlen(name) >= (FILE_NAME_LEN-1)) {
        printf("%s : passed file name exceeds the premitted lenght of %s"
               " bytes\n", FILE_NAME_LEN);
        return FILE_ERR;
    }

    free_inode_bit_array = sb->data.inode_free_list_start_address;
    inode_table = sb->data.inode_arr_start_address;

    /*
     * Find whether the file is already present. If present, just reuturn the
     * inode number.
     */
    for (unsigned int i=0; i<INODE_MAX; i++) {
        if (TEST_BIT_IN_ARRAY(free_inode_bit_array, i)) {
            if (!strcmp(inode_table[i].data.file_name, name)) {
                return i;
            }
        }
    }

    /*
     * Find the free bit in inode free list.
     */
    for (unsigned int i=0; i<INODE_MAX; i++) {
        if (!(TEST_BIT_IN_ARRAY(free_inode_bit_array, i))) {
            free_inode_bit = i;
            break;
        }
    }

    /*
     * Inodes should be in the range of 0 to 'INODE_MAX-1'.
     */
    if (free_inode_bit == NO_FREE_INODE) {
        printf("No free inode in the file system.\n");
        return FILE_ERR;
    }

    /*
     * Populate the inode entry in the inode_table.
     */
    inode_table[free_inode_bit].data.magic = INODE_MAGIC;
    inode_table[free_inode_bit].data.file_size = 0;
    inode_table[free_inode_bit].data.status = FILE_CLOSED;
    strcpy(inode_table[free_inode_bit].data.file_name, name);

    /*
     * Finally, Mark the inode bit TRUE in the inode free list, to represent
     * that this inode is consumed by a file.
     */
    SET_BIT_IN_ARRAY(free_inode_bit_array, free_inode_bit);

    msync(fs_base_address, BYTES_IN_1GB, MS_SYNC|MS_INVALIDATE);

    /*
     * Return the inode bit which got updated.
     */
    return free_inode_bit;
}

/*
 * Return 'FILE_ERR' if fails, inode bit entry on sucess.
 */
int delete (char *name)
{
    super_block *sb = fs_base_address;
    int inode_bit = FILE_NOT_FOUND;
    unsigned char *free_inode_bit_array;
    struct inode *inode_table;

    if (!sb || !name) {
        printf("%s : Wrong values passed.\n", __func__);
        return FILE_ERR;
    }

    if (strlen(name) >= (FILE_NAME_LEN-1)) {
        printf("%s : passed file name exceeds the premitted lenght of %s"
               " bytes\n", FILE_NAME_LEN);
        return FILE_ERR;
    }

    /*
     * Find the inode bit entry for this file.
     */
    inode_table = sb->data.inode_arr_start_address;
    for (unsigned int i=0; i<INODE_MAX; i++) {
        if (!strcmp(inode_table[i].data.file_name, name)) {
            inode_bit = i;
        }
    }

    /*
     * Clear the inode free list bit.
     */
    free_inode_bit_array = sb->data.inode_free_list_start_address;
    if (inode_bit != FILE_NOT_FOUND) {
        CLEAR_BIT_IN_ARRAY(free_inode_bit_array, inode_bit);
    }

    /*
     * Return the inode bit which got updated.
     */
    return inode_bit;
}

/*
 * This function is just to list the files, already existing in the file
 * sysetem.
 */
int list (void)
{
    super_block *sb = fs_base_address;
    unsigned char *free_inode_bit_array;
    struct inode *inode_table;

    if (!sb) {
        printf("%s : Wrong base_address passed.\n", __func__);
        return FILE_ERR;
    }

    free_inode_bit_array = sb->data.inode_free_list_start_address;
    inode_table = sb->data.inode_arr_start_address;

    /*
     * Search for all the occupied inode bits and print the related file names.
     */
    for (unsigned int i=0; i<INODE_MAX; i++) {
        if (TEST_BIT_IN_ARRAY(free_inode_bit_array, i)) {
            printf("%s\n", inode_table[i].data.file_name);
        }
    }

    return 0;
}

/*
 * Below function is just to test the functionalities of file system.
 * Fow now, just doing file create and file delete, but later we can improve on
 * this once file open/close and read/write functions are also present.
 * We can also use hexdump utility on the created 'file' to see the contents of
 * the file system.
 * [ ] hexdump -C file
 */
void test (void *fs_base_address)
{
    char buff[64];

    for (unsigned int i=0; i<INODE_MAX; i++) {
        sprintf(buff, "file%d", i);
        create(buff);
    }
    for (unsigned int i=0; i<INODE_MAX-10; i++) {
        sprintf(buff, "file%d", i);
        delete(buff);
    }
    list();
    return;
}

int main (void)
{
    int fd = 0;
    format(); // Need to do only first time, later we can comment this out.
    printf("format complete.\n");
    fs_base_address = mount(&fd);
    if (!fs_base_address) {
        printf("No file system present.\n");
    }
    test(fs_base_address);
    unmount(fd);
    return 0;
}
