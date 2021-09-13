/*
 * This program is used to demonstrate how we can take the whole file in
 * RAM using mmap, and do the changes. This is very fast compare to the drect
 * file operations done on files, like fgetc, fputc...etc.
 *
 * mmap()  creates  a  new  mapping  in the virtual address space of the
 * calling process. 
 * The starting address for the new mapping is specified in addr.
 * The length argument specifies the length of the
 * mapping (which must be greater than 0).
 */
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    int fd = open("./sometext.txt", O_RDWR, S_IRUSR|S_IWUSR);
    struct stat sb;

    if (fstat(fd, &sb) == -1) {
        perror ("Could not get the file size.\n");
    }

    printf("file size is : %ld\n", sb.st_size);


    char *file_in_memory = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE,
            MAP_SHARED, fd, 0);

    printf("printing file as an array of characters...\n\n");

    for (int i=0; i <sb.st_size; i++) {
        if (isupper(file_in_memory[i])) {
            file_in_memory[i] = tolower(file_in_memory[i]);
        } else {
            file_in_memory[i] = toupper(file_in_memory[i]);
        }
        printf("%c",file_in_memory[i]);
    }
    printf("\n");

    munmap(file_in_memory, sb.st_size);
    close(fd);
}
