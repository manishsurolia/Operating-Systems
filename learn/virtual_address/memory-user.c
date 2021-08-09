#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LINE_LEN 512

/*
 * This program just gets the base address of the same process which is being
 * executed.
 * It just reads the file '/proc/<pid>/maps' and find the start address of the
 * very first segment(usually code segment, because code size does not change
 * during program exeution.
 */
unsigned long long get_process_start_addr(void)
{
    FILE* fp;
    char fname[MAX_LINE_LEN];
    char buffer[MAX_LINE_LEN];
    unsigned long long process_start_addr = 0;

    sprintf(fname, "/proc/%ld/maps", (long)getpid());
    fp = fopen(fname, "r");
    if (fp == NULL) {
      printf("Failed to open process map file: %s \n", fname);
      return 0;
    }
    // Reads first line to get starting address of the process.
    fgets(buffer, MAX_LINE_LEN - 1, fp);
    process_start_addr = strtoll(strtok(buffer, "-"), NULL, 16);

    fclose(fp);
    return process_start_addr;
}

int main(int argc, char **argv)
{
    unsigned long long process_start_addr = get_process_start_addr();
    printf("Address of main function : %p\n",main);
    printf("(Process Start Address + offset) : 0x%llx+0x%llx\n",
            process_start_addr,
            ((unsigned long long)main - process_start_addr));
    while(1);
    return 0;
}
