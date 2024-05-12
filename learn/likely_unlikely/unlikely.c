#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

__attribute__((noinline)) void
print (int input)
{
    if (unlikely(input > 0)) {
        printf("Positive\n");
    } else {
        printf("Zero or Negative\n");
    }
    return;
}

int main(int argc, char *argv[])
{
    int num;
    printf("Please provide a positive/zero/negative number. : ");
    scanf("%d", &num);
    print(num);
    return 0;
}
