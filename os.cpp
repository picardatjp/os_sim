#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include "os.hpp"

// declarations
void loadFile(std::ifstream &f, std::vector<base_operation> &v);
void printpcb(PCB &p);
void createProcesses(std::vector<PCB> &p, int n);
bool runCycle(std::vector<PCB> &pcbs, CPU &cpu);
void printq(std::queue<int> q);

// main????
int main()
{
    srand(time(0));
    bool running_os = true;
    int input;
    std::string file_name;
    // continure asking the user to run file
    while (running_os)
    {
        // set current running process to the first
        CPU cpu;
        cpu.curr_proc_pid = -1;
        cpu.curr_cycle_count = 0;
        std::ifstream myFile;
        // the commands and cycle info
        std::vector<base_operation> base_operations;

        std::cout << "Please choose one of the following:\n[1]: Input file to run.\n[2]: Quit.\n";
        std::cin >> input;
        // if they want to run file
        if (input == 1)
        {
            std::cout << "Enter name of file to run: ";
            std::cin >> file_name;
            myFile.open(file_name);
            if (myFile.is_open())
            {
                // load contents of the file into base_operations
                loadFile(myFile, base_operations);
                // get the number of processes from the file to make
                std::cout << "\nNumber of processes to generate: ";
                std::cin >> input;
                // list of PCBs
                std::vector<PCB> pcbs;
                // create new processes
                for (int i = 0; i < input; i++)
                {
                    // PCB starts out as NEW when made
                    PCB p;
                    p.ps = NEW;
                    // set pid to unique i
                    p.pid = i;
                    // go through each base operation adding it to the operations of the new PCB
                    for (int j = 0; j < base_operations.size(); j++)
                    {
                        int ra = 0;
                        if (base_operations[j].c != FORK)
                            ra = rand() % (base_operations[j].max_cycles - base_operations[j].min_cycles);
                        // int r = randomize(rd) % (base_operations[j].max_cycles - base_operations[j].min_cycles);
                        operation o;
                        o.is_locked = false;
                        // they have same command, just different cycle durations
                        o.c = base_operations[j].c;
                        o.cycles = ra + base_operations[j].min_cycles;
                        // add operation to list of operations within the PCB
                        p.ops.push_back(o);
                    }
                    // add the PCB to the list of PCBs
                    pcbs.push_back(p);
                }
                // set one mutex lock on the very first operation to simulate access to critical section
                // all other processes need to access this critical section, so it just runs until its done basically
                pcbs[0].ops[0].is_locked = true;

                std::cout << "Please choose one of the following schedulers:\n[1]: First come first serve.\n[2]: Round robin.\n";
                std::cin >> input;
                (input == 2) ? round_robin = true
                             : round_robin = false;
                //  for (int i = 0; i < pcbs.size(); i++)
                //  {
                //      printpcb(pcbs[i].ops);
                //  }
                bool running_processes = true;
                // display info menu while processes are running
                while (running_processes)
                {
                    std::cout << "\nPlease choose one of the following:\n[1]: Run x cycles.\n[2]: View current processes in a given state.\n[3]: View cpu queue.\n[4]: Quit\n";
                    std::cin >> input;
                    // user wants to run x cycles
                    if (input == 1)
                    {
                        std::cout << "How many cycles?\n";
                        std::cin >> input;
                        // run x cycles
                        for (int i = 0; i < input; i++)
                        {
                            // if runCycle == false we know there are no more processes to run so we can exit this menu
                            if (!runCycle(pcbs, cpu))
                            {
                                running_processes = false;
                                std::cout << "All processes have ended.\n";
                                break;
                            }
                        }
                    }
                    // user wants to view process info
                    else if (input == 2)
                    {
                        // ask them to view processes within a certain state
                        std::cout << "Which state?\n[1]: NEW\n[2]: READY\n[3]: RUN\n[4]: WAIT\n[5]: EXIT\n";
                        std::cin >> input;
                        // default incase mistype
                        procstate ps = RUN;
                        // set state based on user input
                        if (input == 1)
                            ps = NEW;
                        else if (input == 2)
                            ps = READY;
                        else if (input == 3)
                            ps = RUN;
                        else if (input == 4)
                            ps = WAIT;
                        else if (input == 5)
                            ps = EXIT;
                        else
                            std::cout << "didn't get that, displaying running processes.\n";
                        // flag
                        bool called = false;
                        // go through each PCB, if its state matches that which the user selected, we print it
                        for (int i = 0; i < pcbs.size(); i++)
                        {
                            if (pcbs[i].ps == ps)
                            {
                                printpcb(pcbs[i]);
                                called = true;
                            }
                        }
                        // if we never called the printpcb func, there was nothing to display in the given state
                        if (!called)
                            std::cout << "No processes to display.\n";
                    }
                    else if (input == 3)
                    {
                        printq(cpu.q);
                        std::cout << "current process id running: " << cpu.curr_proc_pid << "\n";
                    }
                    // we set the running flags to false to exit program
                    else if (input == 4)
                    {
                        std::cout << "are you sure?\n[1]: yes, exit.\n[2]: no, don't exit.\n";
                        std::cin >> input;
                        if (input == 1)
                        {
                            std::cout << "shutting down..";
                            running_os = false;
                            running_processes = false;
                        }
                    }
                    else
                        std::cout << "couldn't read that, try again\n";
                }
                // printProcess(base_operations);
            }
            else
            {
                std::cout << "unable to open file\n";
            }
            myFile.close();
        }
        // set running flag to false
        else if (input == 2)
        {
            std::cout << "are you sure?\n[1]: yes, exit.\n[2]: no, don't exit.\n";
            std::cin >> input;
            if (input == 1)
            {
                std::cout << "shutting down..";
                running_os = false;
            }
        }
        else
        {
            std::cout << "didn't recognize that\n";
        }
    }
    return 0;
}

// loads contents of file, f, to vector, v.
void loadFile(std::ifstream &f, std::vector<base_operation> &v)
{
    std::string str;
    int min, max;
    // while no errors occur and not at eof
    while (f.good())
    {
        // get command
        f >> str;

        base_operation b;
        // set the command of base operation
        if (str[0] == 'C')
        {
            // get min cycles
            f >> min;
            // get max cycles
            f >> max;
            b.c = CALCULATE;
        }
        else if (str[0] == 'I')
        {
            // get min cycles
            f >> min;
            // get max cycles
            f >> max;
            b.c = IO;
        }
        else if (str[0] == 'F')
        {
            min = 1;
            max = 2;
            b.c = FORK;
        }
        else
            // should probably return false, saying couldn't load file properly
            std::cout << "didn't recognize command\n";

        // set the cycle info for base operation
        b.min_cycles = min;
        b.max_cycles = max;
        // std::cout << str << " " << min << " " << max << "\n";
        // add base operation to v
        v.push_back(b);
    }
}

// prints PCB info
void printpcb(PCB &p)
{
    // first the process id
    std::cout << "----------------------\n";
    std::cout << "PROCESS #" << p.pid << "\n";
    std::cout << "----------------------\n";
    // next the remaining operations to be performed within the process
    for (operation o : p.ops)
    {
        if (o.c == CALCULATE)
            std::cout << "CALCULATE : ";
        if (o.c == IO)
            std::cout << "I/O : \t";
        if (o.c == FORK)
            std::cout << "FORK : \t";
        std::cout << "\tcycles remaining -> " << o.cycles << "\n";
    }
    std::cout << "----------------------\n";
}

void createProcesses(std::vector<PCB> &p, int n)
{
}

// here is where i was working, if something broke its here

// first come first serve
// runs a single cycle on the CPU
// round robin adds one when RUNNING process gets changed to a process later in the pcbs array, ex. P0 was running now its P1, it goes over it again and does an extra cycle
// not gonna fix it
bool runCycle(std::vector<PCB> &pcbs, CPU &cpu)
{
    // flags
    bool r = true;
    int e = 0;
    // int cnt = 0;
    // go through all PCBs
    for (int i = 0; i < pcbs.size(); i++)
    {
        // if the process id matches the one running on the CPU
        if (pcbs[i].ps == RUN)
        {
            // cnt++;
            // if (cnt >= 2)
            // {
            //     std::cout << "\n\n2 prcoesses running at once..\n\n";
            // }
            // if there are operations remaining in this process
            if (pcbs[i].ops.size() > 0)
            {
                if (pcbs[i].ops[0].c != FORK)
                {
                    // if the cycles on this operation is 0
                    // if (pcbs[i].ops[0].cycles == 0)
                    // {
                    //     std::cout << "problem!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    // }
                    // one cycle has passed since this operation has been running
                    pcbs[i].ops[0].cycles--;

                    if (round_robin && !pcbs[i].ops[0].is_locked)
                    {
                        cpu.curr_cycle_count++;
                        if (rr_q <= cpu.curr_cycle_count)
                        {
                            // std::cout << "round robin\n";
                            // cpu.q.push(cpu.q.front());
                            // cpu.q.pop();
                            // next pid in the queue
                            // will the queue ever be empty?
                            // if (cpu.q.empty())
                            // {
                            //     std::cout << "q was empty?????????????????????????\n";
                            // }
                            cpu.q.push(cpu.curr_proc_pid);

                            cpu.curr_proc_pid = cpu.q.front();
                            cpu.q.pop();
                            for (int j = 0; j < pcbs.size(); j++)
                            {
                                if (cpu.q.back() == pcbs[j].pid)
                                {
                                    // std::cout << "setting " << pcbs[j].pid << " to READY\n\n";
                                    pcbs[j].ps = READY;
                                }
                                if (cpu.curr_proc_pid == pcbs[j].pid)
                                {
                                    // std::cout << "setting " << pcbs[j].pid << " to RUN\n\n";
                                    pcbs[j].ps = RUN;
                                }
                            }
                            // for (PCB pp : pcbs)
                            // {
                            //     if (pp.pid == cpu.curr_proc_pid)
                            //     {
                            //         pp.ps = RUN;
                            //         std::cout << "setting " << pp.pid << " to run\n";
                            //     }
                            //     if (pp.pid == cpu.q.back())
                            //     {
                            //         std::cout << "setting " << pp.pid << " to ready\n";
                            //         pp.ps = READY;
                            //     }
                            // }
                            cpu.curr_cycle_count = 0;
                        }
                    }
                    else
                        cpu.curr_cycle_count = 0;

                    // if the operation has no more cycles to be run
                    if (pcbs[i].ops[0].cycles <= 0)
                    {
                        // we remove the operation from the list of operations in this process
                        pcbs[i].ops.erase(pcbs[i].ops.begin());
                        if (pcbs[i].ops[0].c == IO)
                        {
                            pcbs[i].ps = WAIT;
                            // std::cout << "\n\nsetting wait, currPID= " << cpu.curr_proc_pid << " new pid = " << cpu.q.front() << "\n\n";
                            if (cpu.q.size() > 0)
                            {
                                cpu.curr_proc_pid = cpu.q.front();
                                // std::cout << "this should be 1... : " << cpu.curr_proc_pid << "\n\n";
                                cpu.q.pop();
                                for (int j = 0; j < pcbs.size(); j++)
                                {
                                    if (cpu.curr_proc_pid == pcbs[j].pid)
                                    {
                                        // std::cout << "setting " << pcbs[j].pid << " to RUN\n\n";
                                        pcbs[j].ps = RUN;
                                        break;
                                    }
                                }
                                // set count to zero because we are gonna start running another process
                                cpu.curr_cycle_count = 0;
                            }
                            else
                                cpu.curr_proc_pid = -1;
                        }
                    }
                    // we did run a cycle
                    // std::cout << "\nfirst if\n";
                }
                else
                {
                    std::cout << "\nforking process " << pcbs[i].pid << "\n";
                    // delete the fork call so not infinite loop
                    pcbs[i].ops.erase(pcbs[i].ops.begin());
                    // PCB starts out as NEW when made
                    // std::cout << "spot 1\n";
                    PCB p;
                    p.ps = NEW;
                    // set pid to unique int
                    p.pid = pcbs.size();
                    // std::cout << "spot 2";
                    // go through each base operation adding it to the operations of the new PCB
                    for (int j = 0; j < pcbs[i].ops.size(); j++)
                    {
                        // std::cout << "spot 3";
                        operation o;
                        o.is_locked = pcbs[i].ops[j].is_locked;
                        o.c = pcbs[i].ops[j].c;
                        o.cycles = pcbs[i].ops[j].cycles;
                        // std::cout << "spot 4";
                        // add operation to list of operations within the PCB
                        p.ops.push_back(o);
                    }
                    // std::cout << "spot 5";
                    // add the PCB to the list of PCBs
                    pcbs.push_back(p);
                }
            }
            // we did not run a cycle because there were no operations to run
            // could just be else
            if (pcbs[i].ops.size() <= 0)
            {
                // std::cout << "\n\n\n\nthe else\n\n\n\n";
                // no operations left means we terminate the process
                pcbs[i].ps = EXIT;
                // if there is another process after this we set the cpu to run it
                if ((i < (pcbs.size() - 1)) && (cpu.q.size() > 0))
                {
                    cpu.q.pop();
                    cpu.curr_proc_pid = cpu.q.front();
                    for (int j = 0; j < pcbs.size(); j++)
                    {
                        if (cpu.curr_proc_pid == pcbs[j].pid)
                        {
                            // std::cout << "setting " << pcbs[j].pid << " to RUN\n\n";
                            pcbs[j].ps = RUN;
                            break;
                        }
                    }
                }
                else
                    // no more processes left to run. again, should check q not this
                    r = false;
            }
        }
        // every other process that operations, and the first one is WAIT
        // WAITING processes should advance even when not running on CPU
        if (pcbs[i].ops.size() > 0 && pcbs[i].ps == WAIT)
        {
            // if the first operation has cycles, we remove one
            if (pcbs[i].ops[0].cycles > 1)
                pcbs[i].ops[0].cycles--;
            else
            {
                // no more cyclesin operation = remove operation
                // should check if next operation is IO, then put ps into WAIT or CALC or FORK and put it into READY
                pcbs[i].ops.erase(pcbs[i].ops.begin());
                if (pcbs[i].ops[0].c == CALCULATE || pcbs[i].ops[0].c == FORK)
                {
                    pcbs[i].ps = READY;
                    cpu.q.push(pcbs[i].pid);
                }
            }
        }
        else if (pcbs[i].ps == WAIT)
        {
            // std::cout << "setting " << pcbs[i].pid << " to exit\n";
            pcbs[i].ps = EXIT;
        }
        // if we have a new process, do a bunch of pretend stuff and then make it READY
        if (pcbs[i].ps == NEW)
        {
            // std::cout << "spot 80";
            pcbs[i].ps = READY;
            cpu.q.push(pcbs[i].pid);
        }
        if (cpu.curr_proc_pid == -1 && cpu.q.size() > 0)
        {
            // std::cout << "pid=-1 and run q not empty";
            cpu.curr_proc_pid = cpu.q.front();
            cpu.q.pop();
            for (int j = 0; j < pcbs.size(); j++)
            {
                if (cpu.curr_proc_pid == pcbs[j].pid)
                {
                    // std::cout << "setting " << pcbs[j].pid << " to RUN\n\n";
                    pcbs[j].ps = RUN;
                    break;
                }
            }
        }
        if (pcbs[i].ps == EXIT)
        {
            e++;
        }
        // cnt = 0;
    }
    if (e >= pcbs.size())
        r = false;
    // true -> processes remain
    // false -> no more processes remain
    if (!r)
    {
        for (int i = 0; i < pcbs.size(); i++)
        {
            printpcb(pcbs[i]);
        }
    }
    return r;
}

void printq(std::queue<int> q)
{
    std::cout << "\n---------------------------------\n";
    std::cout << "      CPU QUEUE                  \n";
    int size = q.size();
    for (int i = 0; i < size; i++)
    {
        std::cout << q.front() << ", ";
        q.pop();
    }
    std::cout << "\n---------------------------------\n\n";
}