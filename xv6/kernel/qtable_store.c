#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "defs.h"
#include "fs.h"
#include "qtable_store.h"
#include "stat.h"
#include "file.h"
#include "debug.h"

// 你现在内存里的 Q-table（可以先 extern）
extern int qtable[NSTATE][NACTION];
extern struct spinlock qtable_lock;
extern struct inode* create(char *path, short type, short major, short minor);

// 0 表示 没找到（exception）
// inode 默认不上锁
static struct inode *
qtable_get_inode(int createflag)
{
  struct inode *ip;

  if(createflag){
    ip = create(QTABLE_PATH, T_FILE, 0, 0);

    if(ip == 0)
      return 0;
    iunlock(ip);   // unify: always return unlocked inode
    return ip;
  }
  // DEBUG 
  LOG("before qtable path...\n");
  return namei(QTABLE_PATH);

}

static void
qtable_pack(struct qtable_disk *d)
{
  int i, j;

  d->magic = QTABLE_MAGIC;
  d->version = QTABLE_VERSION;
  d->nstate = NSTATE;
  d->naction = NACTION;

  // 这里是不是应该掉 qtable 的锁
  acquire(&qtable_lock);
  for(i = 0; i < NSTATE; i++){
    for(j = 0; j < NACTION; j++){
      d->q[i][j] = qtable[i][j];
    }
  }
  release(&qtable_lock);
}


// save 成功返回 0 
// save 不成功返回 - 1
int
qtable_save(void)
{
  struct inode *ip;
  struct qtable_disk d;
  int n;

  // 1. 把内存中的 qtable 打包到磁盘结构
  qtable_pack(&d);

  // 2. 开启事务
  begin_op();

  // 3. 获取 inode（必要时创建）

  // 这里 qtable_get_inode 是不是应该返回带锁的，防止inode同时访问会被释放？
  // 而不是回来以后再加锁。
  ip = qtable_get_inode(1);
  if(ip == 0){
    end_op();
    LOG("qtable get inode failed!!!\n");
    return -1;
  }

  // 4. 加锁 inode
  ilock(ip);

  // 5. 写入整个 qtable_disk
  n = writei(ip, 0, (uint64)&d, 0, sizeof(d));
  if(n != sizeof(d)){
    iunlockput(ip);
    end_op();
    return -1;
  }

  // 6. 解锁并释放 inode
  iunlockput(ip);

  // 7. 结束事务
  end_op();

  return 0;
}

static int
qtable_validate(struct qtable_disk *d)
{
  if(d->magic != QTABLE_MAGIC)
    return -1;
  if(d->version != QTABLE_VERSION)
    return -1;
  if(d->nstate != NSTATE)
    return -1;
  if(d->naction != NACTION)
    return -1;
  return 0;
}

static void
qtable_unpack(struct qtable_disk *d)
{
  int i, j;

  acquire(&qtable_lock);
  for(i = 0; i < NSTATE; i++){
    for(j = 0; j < NACTION; j++){
      qtable[i][j] = d->q[i][j];
    }
  }
  release(&qtable_lock);
}

void qtable_init_default()
{
    // do noting
    // 这个地方可以更改 qtable 的初始化逻辑
}

// qtable 的 init 是需要 init 锁的
// 你这个 qtable load 是有可能返回 -1 的，也就是可能会初始化失败。
// qtable_load 不负责处理错误情况, 直接返回 -1
int
qtable_load(void)
{
  struct inode *ip;
  struct qtable_disk d;
  int n;

  // 1. 先查找，不强制创建
  // DEBUG
  LOG("before qtable get inode...\n");
  ip = qtable_get_inode(0);
  if(ip == 0){
    goto fail;
  }

  // 2. 开启事务，读取文件
  begin_op();

  ilock(ip);
  n = readi(ip, 0, (uint64)&d, 0, sizeof(d));
  iunlockput(ip);

  end_op();

  // 3. 读取长度不对，视为失败
  // 4. 校验磁盘格式
  if(n != sizeof(d) || qtable_validate(&d) < 0){
    goto fail;
  }

  // 5. 合法则加载进内存
  qtable_unpack(&d);
  return 0;

 fail:
    printf("qtable load falied!");
    qtable_init_default();
    qtable_save();
    return -1;
}

uint64 sys_saveqtable(void)
{
  return qtable_save();
}