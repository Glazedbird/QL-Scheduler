#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// ⚠️ 必须和内核里完全一致
#define NSTATE 9
#define NACTION 4

#define QTABLE_MAGIC 0x5154424c
#define QTABLE_VERSION 1

struct qtable_disk {
  uint magic;
  uint version;
  uint nstate;
  uint naction;
  int q[NSTATE][NACTION];
};

int
main(int argc, char *argv[])
{
  int fd;
  struct qtable_disk d;
  int i, j;

  fd = open("/qtable.bin", 0);
  if(fd < 0){
    printf("showqtable: cannot open /qtable.bin\n");
    exit(1);
  }

  int n = read(fd, &d, sizeof(d));
  if(n != sizeof(d)){
    printf("showqtable: read failed, n=%d\n", n);
    close(fd);
    exit(1);
  }

  close(fd);

  // ===== 校验头 =====
  if(d.magic != QTABLE_MAGIC){
    printf("invalid magic: 0x%x\n", d.magic);
    exit(1);
  }

  if(d.version != QTABLE_VERSION){
    printf("invalid version: %d\n", d.version);
    exit(1);
  }

  if(d.nstate != NSTATE || d.naction != NACTION){
    printf("dimension mismatch: nstate=%d naction=%d\n",
           d.nstate, d.naction);
    exit(1);
  }

  // ===== 打印 =====
  printf("Q-table (from disk):\n");

  for(i = 0; i < d.nstate; i++){
    printf("state %d: ", i);
    for(j = 0; j < d.naction; j++){
      printf("%d ", d.q[i][j]);
    }
    printf("\n");
  }

  exit(0);
}