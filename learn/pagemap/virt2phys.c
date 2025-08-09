#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int global_data;

#define PAGEMAP_ENTRY 8  // Each entry is 8 bytes
#define PFN_MASK     ((1ULL << 55) - 1)

int main(int argc, char *argv[])
{
    unsigned long virt_addr = (unsigned long)&global_data;
    char pagemap_path[64];
    int fd;
    uint64_t entry;
    ssize_t bytes_read;
    long page_size = sysconf(_SC_PAGESIZE);
    unsigned long page_index = virt_addr / page_size;
    unsigned long offset = page_index * PAGEMAP_ENTRY;


    fd = open("/proc/self/pagemap", O_RDONLY);
    lseek(fd, offset, SEEK_SET);
    bytes_read = read(fd, &entry, PAGEMAP_ENTRY);
    close(fd);

    if (!(entry & (1ULL << 63))) {
        printf("Page not present in RAM.\n");
        return EXIT_SUCCESS;
    }

    unsigned long pfn = entry & PFN_MASK;
    unsigned long phys_addr = (pfn * page_size) + (virt_addr % page_size);

    printf("Virtual address: 0x%lx\n", virt_addr);
    printf("PFN: 0x%lx\n", pfn);
    printf("Physical address: 0x%lx\n", phys_addr);

    return EXIT_SUCCESS;
}
