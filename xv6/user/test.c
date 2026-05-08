#include "kernel/types.h"
#include "user/user.h"

void
print_waitstat(int pid, int status, struct pstat *st)
{
  uint64 resp;
  uint64 turn;

  printf("pid=%d status=%d ", pid, status);

  printf("run=%ld wait=%ld sleep=%ld sched=%ld ",
    st->run_ticks,
    st->wait_ticks,
    st->sleep_ticks,
    st->sched_count);

  if(st->ctime)
    printf("ctime=%ld ", st->ctime);
  else
    printf("ctime=- ");

  if(st->first_run_time)
    printf("first=%ld ", st->first_run_time);
  else
    printf("first=- ");

  if(st->etime)
    printf("etime=%ld ", st->etime);
  else
    printf("etime=- ");

  if(st->ctime && st->first_run_time){
    resp = st->first_run_time - st->ctime;
    printf("resp=%ld ", resp);
  } else {
    printf("resp=- ");
  }

  if(st->ctime && st->etime){
    turn = st->etime - st->ctime;
    printf("turn=%ld", turn);
  } else {
    printf("turn=-");
  }

  printf("\n");
}

int
main(void)
{
  int status;
  struct pstat st;

  int pid = fork();
  if(pid == 0){
    pause(100);
    exit(3);
  }

  int ret = waitstat(&status, &st);
	print_waitstat(ret, pid, &st);

  
  exit(0);
}