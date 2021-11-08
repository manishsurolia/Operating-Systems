#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define ROW 10000
#define COLUMN 10000

#define BILLION 1000000000L
int arr[ROW][COLUMN];

int main(void)
{
    struct timespec tstart={0,0}, tend={0,0};
    uint64_t total = 0;
    uint64_t time = 0;

    // Just set some value in each entry of the matrix.
    for (unsigned int i = 0; i < ROW; i++) {
        for (unsigned int j = 0; j < COLUMN; j++) {
            arr[i][j] = 1;
        }
    }

    // Sequential Read
    tstart.tv_sec = tstart.tv_nsec = tend.tv_sec = tend.tv_nsec = 0;
    total = time = 0;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    for (unsigned int i = 0; i < ROW; i++) {
        for (unsigned int j = 0; j < COLUMN; j++) {
            total = total + arr[i][j];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &tend);
    time = (BILLION * (tend.tv_sec - tstart.tv_sec)) +
           (tend.tv_nsec - tstart.tv_nsec);
    printf("(Sequential READS in 2-D Array) :\n");
    printf("Result : %lld, Total time taken : %lld nanoseconds\n",
           (long long unsigned int)total, (long long unsigned int)time);

    // Non-Sequential Read
    tstart.tv_sec = tstart.tv_nsec = tend.tv_sec = tend.tv_nsec = 0;
    total = time = 0;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    for (unsigned int i = 0; i < ROW; i++) {
        for (unsigned int j = 0; j < COLUMN; j++) {
            total = total + arr[j][i];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &tend);
    time = (BILLION * (tend.tv_sec - tstart.tv_sec)) +
           (tend.tv_nsec - tstart.tv_nsec);
    printf("\n(Non-Sequential READS in 2-D Array) :\n");
    printf("Result : %lld, Total time taken : %lld nanoseconds\n",
           (long long unsigned int)total, (long long unsigned int)time);

    return 0;
}
