#ifndef OS_H
#define OS_H

#include <vector>
#include <queue>

bool round_robin = false;
int rr_q = 5;

typedef enum command
{
    CALCULATE,
    IO,
    FORK
} command;

typedef enum procstate
{
    NEW,
    READY,
    RUN,
    WAIT,
    EXIT
} procstate;

typedef struct operation
{
    bool is_locked;
    command c;
    int cycles;
} operation;

typedef struct base_operation
{
    command c;
    int min_cycles, max_cycles;
} base_operation;

typedef struct PCB
{
    int pid_of_child;
    int pid;
    procstate ps;
    std::vector<operation> ops;
} PCB;

typedef struct CPU
{
    int curr_cycle_count;
    int curr_proc_pid;
    std::queue<int> q;
} CPU;

#endif