#ifndef MEMORY_H
#define MEMORY_H

#include "process.h"

#define MEM_SIZE 60
#define MAX_VARIABLES 3

typedef struct {
    char* key;   // e.g., "pid", "instruction"
    char* value; // e.g., "1", "printFromTo 1 5"
} MemoryWord;

typedef struct {
    MemoryWord words[MEM_SIZE];
    int used[MEM_SIZE]; // 0 = free, 1 = used
} MemoryManager;

// Memory functions
void init_memory(MemoryManager* mem);
int allocate_process(MemoryManager* mem, PCB* pcb, char** program, int program_len);
void increment_program_counter_mem(MemoryManager* mem, PCB* pcb);
void update_pcb_state_mem(MemoryManager* mem, PCB* pcb, ProcessState new_state);
void free_process(MemoryManager* mem, PCB* pcb);
void print_memory(MemoryManager* mem);
char* state_to_string(PCB* pcb);

#endif