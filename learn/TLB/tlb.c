/*
 * Purpose of this program is to see the effect of TLB in Linux OS.
 *
 * If the TLB can hold 32 VPN->PFN translations, then till 32 pages repeatedly
 * access(10000000 times), 'Average access time per page' is less and as soon as
 * we increase the 'NUMPAGES' to 33, the 'access time per page' will increase
 * because the translation will not be found in the TLB, and OS need to go to
 * process page table entry to get the related physical page number.
 *
 * You can just increase the value of 'NUMPAGES' slowly and
 *
 * Note : The only value to change here is 'NUMPAGES', to see how 'average
 * access time per page' varies with more number of pages because TLB has a
 * limit(32/64/128 entries max).
 *
 * For complete details about paging and TLB see the related README files in the
 * same directory here.
 *
 * Note : This program was written and tested in Ubuntu linux VM in 'virtual
 * box', and it shows almost same 'average access time per page' for any number
 * of 'NUMPAGES', which gives an impression that in a linux VM, VPN->PFN
 * translations are always done with the use of paging and there is no
 * involvement of any TLB here.
 */

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define BILLION 1000000000L
#define NUMPAGES 32 // Number of pages used by our array 'arr'
#define PAGESIZE 4096 // Taken from this command of linux 'getconf PAGE_SIZE'
#define ARR_SIZE ((PAGESIZE / sizeof(int)) * NUMPAGES)
#define REPEAT_FACTOR 10000000

int arr[ARR_SIZE];

#pragma GCC optimize ("O0")
int main(int argc, char **argv)
{
    unsigned int jump = PAGESIZE / sizeof(int); // To jump from page to the next
    struct timespec tstart={0,0}, tend={0,0};
    uint64_t total = 0;
    uint64_t average = 0; //Average time(in ns), per page access

    /*
     * We want to update each page in the array REPEAT_FACTOR times.
     */
    for (unsigned int i = 0; i < REPEAT_FACTOR; i++) {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        /*
         * Access each page used by this array, and just update the first few
         * bytes (sizeof(int)) of the page.
         */
        for (unsigned int j = 0; j < (NUMPAGES * jump); j = (j + jump)) {
            arr[j] += 1;
        }
        clock_gettime(CLOCK_MONOTONIC, &tend);
        total = total + (BILLION * (tend.tv_sec - tstart.tv_sec)) +
                (tend.tv_nsec - tstart.tv_nsec);
        tstart.tv_sec = tstart.tv_nsec = tend.tv_sec = tend.tv_nsec = 0;
    }
    average = (total / REPEAT_FACTOR) / NUMPAGES;
    printf("\nAverage Time (Per page access) : %llu nanoseconds\n",
            (long long unsigned int)average);

    return 0;
}
