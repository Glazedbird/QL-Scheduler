// kernel/rand.c
#include "types.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

static struct spinlock rand_lock;
static uint rand_state = 2463534242U;

void
randinit(uint seed)
{
  initlock(&rand_lock, "rand");
  if(seed != 0)
    rand_state = seed;
}

void
srand(uint seed)
{
  acquire(&rand_lock);
  if(seed != 0)
    rand_state = seed;
  release(&rand_lock);
}

uint
rand32(void)
{
  uint x;

  acquire(&rand_lock);
  x = rand_state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  rand_state = x;
  release(&rand_lock);

  return x;
}

int
rand_range(int n)
{
  if(n <= 0)
    return 0;
  return rand32() % n;
}