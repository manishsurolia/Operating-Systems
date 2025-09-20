#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHUNK_SIZE (1024 << 20) // 1024 MB = 1 GB per step

// Read VmSwap (in kB) for this process from /proc/self/status
long get_vmswap_kb(void) {
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return -1;

    char line[256];
    long value = -1;

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "VmSwap:", 7) == 0) {
            sscanf(line + 7, "%ld", &value);
            break;
        }
    }

    fclose(f);
    return value; // in kB
}

int main(void) {
    printf("Allocating memory in 1GB chunks... (press Ctrl+C to stop)\n");

    size_t total_alloc = 0;

    while (1) {
        char *chunk = malloc(CHUNK_SIZE);
        if (!chunk) {
            perror("malloc");
            break;
        }

        // Touch the memory so it's actually committed
        memset(chunk, 0xAB, CHUNK_SIZE);

        total_alloc += CHUNK_SIZE;

        long swap_kb = get_vmswap_kb();
        if (swap_kb >= 0)
            printf("Allocated %zu MB, VmSwap = %ld kB\n", total_alloc >> 20, swap_kb);
        else
            printf("Allocated %zu MB, VmSwap = (unavailable)\n", total_alloc >> 20);

        sleep(1); // short delay to let system catch up
    }

    return 0;
}

