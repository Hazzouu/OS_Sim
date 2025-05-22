#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "memory.h"
#include "queue.h"

typedef enum{
    FCFS,
    RR,
    MLFQ
}selected_schedule;


extern int os_clock;
extern int programs;
extern int arr_index;
extern Queue lvl1;
extern Queue lvl2;
extern Queue lvl3;
extern Queue lvl4;
extern Queue ready_queue;
extern selected_schedule schedule;
extern bool new_arrival;
extern int quanta;
extern int current_quanta;
extern PCB* pcbs_list[50];
extern char* filepaths[50];

void fifo_scheduler(MemoryManager* memory, Queue* ready_queue);
void round_robin(MemoryManager* mem , Queue* ready_queue);
void multilevel_feedback_queue(MemoryManager* mem, Queue* level1, Queue* level2, Queue* level3, Queue* level4);
void execute_level(MemoryManager* mem, Queue* current_level, Queue* next_level, int quantum);
void init_quanta();
int all_blocked(Queue* q);
void update(MemoryManager* mem);

#endif