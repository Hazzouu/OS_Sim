// mutex.h
#ifndef MUTEX_H
#define MUTEX_H

#include "queue.h"
#include "process.h" 
#include "memory.h" 
#include "scheduler.h"



typedef struct {
    int pid;
    int locked;       // 0 = unlocked, 1 = locked
    Queue waitingQ;   // Queue of blocked processes (you can use PCB* or int for PID)
} Mutex;

extern Mutex file;
extern Mutex input;
extern Mutex output;

void initMutex(Mutex* m);
void semWait(MemoryManager*mem,Mutex* m, PCB* pcb);
void semSignal(MemoryManager*  mem, Mutex* m, PCB* pcb);

#endif // MUTEX_H
