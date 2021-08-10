/*
 * Purpose of this program is to see the effect of TLB in Linux OS.
 *
 * If the TLB can hold 32 VPN->PFN translations, then till 32 pages repeatedly
 * access(10000000 times), 'Average access time per page' is less and as soon as
 * we increase the 'NUMPAGES' to 33, the 'access time per page' will increase
 * because the translation will not be found in the TLB, and OS need to go to
 * process page table entry to get the related physical page number.
 *
 * You can just increase the value of 'NUMPAGES' one by one and find where you
 * see sharp rise in 'Average access time per page', this will reflect the size
 * of TLB.
 *
 * Note : The only value to change here is 'NUMPAGES', to see how 'average
 * access time per page' varies with more number of pages because TLB has a
 * limit(32/64/128 entries max).
 *
 * Note : This program runs only on a single core on the system. Achieved with
 * 'sched_setaffinity' system call.
 *
 * For complete details about paging and TLB see the related README files in the
 * same directory here.
 *
 * Note : This program was written and tested in Ubuntu linux VM in 'virtual
 * box', and below were the 'average access time per page' with different number
 * of pages accessed.
 *
 * With 'NUMPAGES = 32',   'average access time per page' is : 9 nano seconds.
 * With 'NUMPAGES = 1024', 'average access time per page' is : 11 nano seconds.
 * With 'NUMPAGES = 2048', 'average access time per page' is : 17 nano seconds.
 *
 * This shows that the TLB in this VM supports VPN->PFN traslations between
 * 1024 to 2048. More entries than that, cause increase in the access time,
 * because OS need to check the page table.
 */


#define _GNU_SOURCE
#include <sched.h> /* To schedule this program on a single core */

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define BILLION 1000000000L
#define NUMPAGES 32 /* Number of pages used by our array 'arr' */
#define PAGESIZE 4096 /* Taken from this command of linux 'getconf PAGE_SIZE' */
#define ARR_SIZE ((PAGESIZE / sizeof(int)) * NUMPAGES)
#define REPEAT_FACTOR 10000000 /* To update all pages this many times */

int arr[ARR_SIZE];

#pragma GCC optimize ("O0")
int main(int argc, char **argv)
{
    unsigned int jump = PAGESIZE / sizeof(int); /* To jump one page to next */
    struct timespec tstart={0,0}, tend={0,0};
    uint64_t total = 0;
    uint64_t average = 0; /* Average time(in ns), per page access */

    /*
     * Forcing this process to run on a single processer, because each processor
     * has a TLB with it, and we want to test a single processor TLB behaviour.
     */
    cpu_set_t my_set;
    CPU_ZERO(&my_set);
    CPU_SET(0, &my_set);
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

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
    printf("Average Time (Per page access) : %llu nanoseconds\n",
            (long long unsigned int)average);

    return 0;
}
