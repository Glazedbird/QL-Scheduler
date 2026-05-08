#ifndef QTABLE_STORE_H
#define QTABLE_STORE_H

#include "types.h"
#include "rl.h"

#define QTABLE_MAGIC 0x5154424c
#define QTABLE_VERSION 1
#define QTABLE_PATH  "/qtable.bin"

struct qtable_disk {
  uint magic;
  uint version;
  uint nstate;
  uint naction;
  int q[NSTATE][NACTION];
};

// 对外接口
int qtable_load(void);
int qtable_save(void);

#endif