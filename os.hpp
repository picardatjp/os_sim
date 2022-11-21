#ifndef OS_H
#define OS_H

#include <vector>
#include <queue>

bool round_robin = false;
int rr_q = 5;
/***************************
 * instruction = 1 page
 * 1 page = 1 mb
 * total possible number of pages is 512
 * 512 frames, 1 frame = 1 page
 */

const int MAX_MEM = 512;

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
    int p_index;
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
    // add memory needed for this process
    int mem_req;
    std::vector<std::pair<int, int>> pt;
} PCB;

typedef struct CPU
{
    int curr_cycle_count;
    int curr_proc_pid;
    std::queue<int> q;
    // total used memory on queue
    int total_mem;
} CPU;

// represented in frames/pages
typedef struct memory
{
    int m[MAX_MEM];
} memory;

#endif