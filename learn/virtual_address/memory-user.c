#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LINE_LEN 512

/*
 * This program just gets the base address of the same process which is being
 * executed.
 * It just reads the file '/proc/<pid>/maps'
 */
unsigned long long get_base_addr(void)
{
    FILE* fp;
    char fname[MAX_LINE_LEN];
    char buffer[MAX_LINE_LEN];
    unsigned long long base_addr = 0;

    sprintf(fname, "/proc/%ld/maps", (long)getpid());
    fp = fopen(fname, "r");
    if (fp == NULL) {
      printf("Failed to open process map file: %s \n", fname);
      return 0;
    }
    // Reads first line to get starting address of the process.
    fgets(buffer, MAX_LINE_LEN - 1, fp);
    base_addr = strtoll(strtok(buffer, "-"), NULL, 16);

    fclose(fp);
    return base_addr;
}

int main(int argc, char **argv)
{
    unsigned long long base_addr = get_base_addr();
    printf("Address of main function : %p\n",main);
    printf("(Process Base Address + offset) : 0x%llx+0x%llx\n", base_addr,
           ((unsigned long long)main - base_addr));
    while(1);
    return 0;
}
