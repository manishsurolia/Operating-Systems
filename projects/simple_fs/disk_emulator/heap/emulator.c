#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../common/include/emulator.h"

void * start_address;

/*
 * disk_init() just reserves 64 blocks(each block is of size 4KB) of 
 * memory(in heap) to behave like a hard disk memory. The read/write functions 
 * on this simulated hard disk can done only sector wise. Each sector is 512 
 * bytes long. 
 *
 * Input  : Nothing. It just reservers 64 blocks of memory in heap. 
 * Return : return TRUE if disk initialization is successful, else return FALSE.
 */
uchar disk_init()
{
    start_address = NULL;
    start_address = malloc(DISK_SIZE); 
    if(!start_address) {
        fprintf(stderr, "malloc failed. Error : %s\n", strerror(errno));
        return FALSE;
    }

    memset(start_address, 0, DISK_SIZE);
    return TRUE;
}

/*
 * disk_read() performs read(on a particular sector in hard disk) operation.
 * Input  : 1. sector, from where data needs to be read.
 *        : 2. space to store data, which is read from simulated hard disk from 
 *          the provided sector. 
 * Output : TRUE/FALSE, based on whether read was sucessful or not.
 */ 
uchar disk_read(ulong sector, uchar data[])
{
    uchar * temp;
    if(sector > (TOTAL_SECTORS - 1)) {
        return FALSE;
    }
    temp = start_address;
    memcpy(data, (temp + (SECTOR_SIZE*sector)),
        SECTOR_SIZE);
    return TRUE;
}

/*
 * disk_write() performs write(on a particular sector in hard disk) operation.
 * Input  : 1. sector, which needs to be written in the disk.
 *        : 2. data of SECTOR_SIZE, which needs to be written in the provided 
 *             sector.
 * Output : TRUE/FALSE, based on whether writing was sucessful or not.
 */ 
uchar disk_write(ulong sector, uchar data[])
{
    uchar *temp;
    if(sector > (TOTAL_SECTORS - 1)) {
        return FALSE;
    }
    temp = start_address;
    memcpy((temp + (SECTOR_SIZE*sector)), data, 
            SECTOR_SIZE);
    return TRUE;
}

