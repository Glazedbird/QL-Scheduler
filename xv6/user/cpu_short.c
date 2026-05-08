#include "kernel/types.h"
#include "user/user.h"

int main()
{
    volatile int x = 0;
    printf("cpu_short begins!\n");
    for(int i = 0; i < 20000000; i++)
        x += i;

    exit(0);
}