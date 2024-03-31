#include <stdio.h>

unsigned int global_var;

int main() {
    printf("'global_var' address : %p\n", &global_var);
    return 0;
}
