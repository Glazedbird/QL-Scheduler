# QL-Scheduler

基于 Q-learning 的 xv6 轻量级强化学习调度器

## 项目简介

QL-Scheduler 是一个基于 xv6-riscv 实现的强化学习操作系统调度实验项目，旨在探索强化学习方法在操作系统内核调度中的可行性。

本项目在 xv6 默认 Round Robin 调度机制基础上，引入 Q-learning 调度策略，将进程调度过程抽象为马尔可夫决策过程（MDP），并在内核中实现状态构造、动作选择、奖励更新与 Q 表学习机制。

项目同时构建了调度实验与性能分析框架，用于对比传统调度算法与强化学习调度算法在不同负载场景下的行为表现。

---

## 项目特点

- 基于 xv6-riscv 的内核级调度器改造
- 引入 Q-learning 强化学习调度机制
- 将调度过程建模为状态—动作—奖励结构
- 基于固定点整数实现轻量级 Q 表更新
- 支持 ε-greedy 动作探索策略
- 支持 Q-table 持久化存储
- 支持 RR / Priority / QL 三种调度策略对比
- 构建多种 workload 实验场景
- 支持调度统计信息采集与性能分析

---

## 调度流程

调度主要流程集中在[scheduler()](https://github.com/Glazedbird/QL-Scheduler/blob/main/xv6/kernel/proc.c#L511-L556)
```text
Timer Interrupt
      ↓
build_global_state()
      ↓
choose_action()
      ↓
pick_proc_by_action()
      ↓
context switch
      ↓
build_global_state()
      ↓
compute_reward()
      ↓
update_qtable()
```
系统根据当前可运行进程数量、等待时间等状态信息构建调度状态，并通过 Q-learning 算法动态选择调度动作。


## 状态空间设计

当前实现采用离散状态建模：

- RUNNABLE 进程数量 bucket
- 最大等待时间 bucket

状态通过 bucket 编码为有限状态空间。

## 动作空间设计

当前调度动作包括：

- 选择等待时间最长的进程
- 选择被调度次数最少的进程
- 选择累计运行时间最短的进程
- 使用 Round Robin 方式调度

## 主要项目结构

[proc.h](xv6/kernel/proc.h)：对`proc`结构体进行了数据扩展

[rl.c](xv6/kernel/rl.c)：Q-learning 本体核心逻辑

[qtable_store.c](xv6/kernel/qtable_store.c)：Q-table 持久化模块

[rand.c](xv6/kernel/rand.c)：随机数模块，用以支持Qlearning随机探索

[proc.c](xv6/kernel/proc.c)：调度机制主要实现

[workload.c](xv6/user/workload.c)：测试用例构造

[showqtable.c](xv6/user/showqtable.c)：查看q表具体内容

## 构建与运行
### 环境要求
- Linux / WSL2
- QEMU
- RISC-V GCC Toolchain

### 编译运行
```
make qemu
```
### GDB 调试
```
make qemu-gdb
```

## 实验场景

当前实验包括：
- CPU-intensive workload
- mixed workload
- sleep-heavy workload

主要性能指标：

- 平均等待时间（Waiting Time）
- 平均响应时间（Response Time）

### 实验结果

实验结果表明：

强化学习调度器能够根据系统状态动态调整调度行为
在部分 workload 下表现出一定自适应能力
系统存在较明显的性能波动现象
在当前状态空间与奖励设计下，RL 调度尚未稳定优于传统 RR 调度

该结果反映了强化学习在动态操作系统环境中的非稳定性问题，也说明状态建模与奖励函数设计对系统性能具有重要影响。

### 项目目标

本项目主要用于：

- 操作系统调度机制学习
- 强化学习与系统结合实验
- xv6 内核研究
- AI + Operating System 调度探索
- 教学与研究实验平台构建

### 当前局限性

当前实现仍存在一些限制：

- 状态空间较小，表达能力有限
- 使用 Tabular Q-learning，扩展性较弱
- 奖励函数较简单
- 系统环境存在较强非平稳性
- 学习过程容易产生性能震荡

后续可能尝试：

- DQN / Deep RL
- 更复杂状态建模
- 多核调度
- 异构计算环境调度
- 更稳定的奖励机制设计

## Acknowledgments

This project is built upon the xv6-riscv teaching operating system developed by MIT PDOS Lab.

Original repository:
https://github.com/mit-pdos/xv6-riscv

The Q-learning scheduler, benchmarking framework, and kernel extensions were independently implemented in this project for research and educational purposes.
