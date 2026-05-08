#include "kernel/types.h"
#include "user/user.h"

int main()
{
    volatile int x = 0;

    for(int r = 0; r < 50; r++)
    {
        for(int i = 0; i < 1000000; i++)
            x += i;

        if(r % 5 == 0)
            pause(5);
    }

    exit(0);
}