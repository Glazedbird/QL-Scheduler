#include "rl.h"
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "qtable_store.h"

int qtable[NSTATE][NACTION];
struct spinlock qtable_lock;

void qtableinit() {
    initlock(&qtable_lock, "qtable_lock");
    qtable_load();
}

int
state_runnable_bucket(int cnt)
{
  if(cnt <= 1)
    return 0;
  if(cnt <= 4)
    return 1;
  return 2;
}

int
state_maxwait_bucket(uint64 max_wait)
{
  if(max_wait < 5)
    return 0;
  if(max_wait < 20)
    return 1;
  return 2;
}

int
choose_action(int state)
{
  if(state < 0 || state >= NSTATE)
    return 0;

  if(rand_range(100) < EPOSILON)
    return rand_range(NACTION);

  int best = 0;

  acquire(&qtable_lock);
  for(int a = 1; a < NACTION; a++){
    if(qtable[state][a] > qtable[state][best])
      best = a;
  }
  release(&qtable_lock);

  return best;
}

void
rl_update(int s, int a, int s2, int reward, int terminal)
{
  int old_q;
  int max_next;
  int target;
  int new_q;

  if(s < 0 || s >= NSTATE)
    panic("rl_update: bad s");
  if(a < 0 || a >= NACTION)
    panic("rl_update: bad a");
  if(s2 < 0 || s2 >= NSTATE)
    panic("rl_update: bad s2");

  acquire(&qtable_lock);

  old_q = qtable[s][a];

  max_next = qtable[s2][0];
  for(int i = 1; i < NACTION; i++){
    if(qtable[s2][i] > max_next)
      max_next = qtable[s2][i];
  }

  if(terminal)
    target = reward * SCALE;
  else
    target = reward * SCALE + (GAMMA * max_next) / SCALE;

  new_q = old_q + (ALPHA * (target - old_q)) / SCALE;
  qtable[s][a] = new_q;

  release(&qtable_lock);
}

int
compute_reward(int old_state, int next_state, int terminal)
{
  int reward = 0;

  int old_wait_bucket = old_state % NWAIT_BUCKET;
  int next_wait_bucket = next_state % NWAIT_BUCKET;

  if(terminal)
    reward += 5;

  if(next_wait_bucket < old_wait_bucket)
    reward += 2;
  else if(next_wait_bucket > old_wait_bucket)
    reward -= 2;

  return reward;
}

void
qtabledump(void)
{
  printf("===== qtable dump =====\n");

  acquire(&qtable_lock);
  for(int rb = 0; rb < NRUN_BUCKET; rb++){
    for(int wb = 0; wb < NWAIT_BUCKET; wb++){
      int idx = rb * NWAIT_BUCKET + wb;
      printf("state(rb=%d,wb=%d)[%d]: ", rb, wb, idx);

      for(int a = 0; a < NACTION; a++){
        printf("a%d=%d ", a, qtable[idx][a]);
      }
      printf("\n");
    }
  }
  release(&qtable_lock);

  printf("=======================\n");
}