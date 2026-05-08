#include "kernel/types.h"
#include "user/user.h"

int main()
{
    volatile int x = 0;
    printf("Sleepy job begins!\n");
    for(int r = 0; r < 20; r++)
    {
        for(int i = 0; i < 2000000; i++)
            x += i;

        pause(3);
    }

    exit(0);
}