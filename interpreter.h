#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "process.h"
#include "memory.h"
#include "mutex.h"

void execute_instruction(MemoryManager* mem,PCB* pcb);
char* get_current_instruction(MemoryManager* mem, PCB* pcb);

#endif