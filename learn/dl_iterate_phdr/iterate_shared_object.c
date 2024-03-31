#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <link.h>

unsigned int global_var;

int callback(struct dl_phdr_info *info, size_t size, void *data) {
    printf("Name: %s\n", info->dlpi_name);
    printf("Base address: 0x%lx\n", (unsigned long)info->dlpi_addr);
    printf("\n---------------------\n\n");
    return 0;
}

int main() {
    dl_iterate_phdr(callback, NULL);
    printf("global var address : %p\n", &global_var);
    return 0;
}
