#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE (4 * 1024)
#define DISK_SIZE (BLOCK_SIZE * 64)

/*
 * disks are not byte addressable. It is read at sectors.
 * In this simulation of hard disk, the size of each sector is 512 bytes.
 *
 * The total count of sector is = DISK_SIZE/SECTOR_SIZE
 *                              = 262144/512 = 512 sectors.
 *
 * Hence, our simulated hard disk has total 512 sectors.
 *
 */
#define SECTOR_SIZE 512
#define TOTAL_SECTORS (DISK_SIZE/SECTOR_SIZE)

#define FALSE 0
#define TRUE 1

typedef unsigned char uchar;
typedef unsigned long ulong;

uchar disk_init();
uchar disk_read(ulong sector, uchar data[]);
uchar disk_write(ulong sector, uchar data[]);
