#include "kernel/types.h"
#include "user/user.h"

int main()
{
    volatile int x = 0;
    printf("cpu_long begins!\n");
    for(int i = 0; i < 200000000; i++)
        x += i;

    exit(0);
}