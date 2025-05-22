#include "memory.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void init_memory(MemoryManager* mem) {
    for (int i = 0; i < MEM_SIZE; i++) {
        mem->words[i].key = NULL;
        mem->words[i].value = NULL;
        mem->used[i] = 0;
    }
}

const char* ColorStrings[] = {
    "READY",
    "RUNNING",
    "BLOCKED",
    "TERMINATED",
    "NOT_IN_SYSTEM"
};

int allocate_process(MemoryManager* mem, PCB* pcb, char** program, int program_len) {
    // Calculate needed space: program + vars + PCB fields (6 words)
    int needed = program_len + MAX_VARIABLES + 6;
    
    // Find contiguous block
    int start = -1, consecutive = 0;
    for (int i = 0; i < MEM_SIZE; i++) {
        if (!mem->used[i]) {
            if (start == -1) start = i;
            if (++consecutive >= needed) break;
        } else {
            start = -1;
            consecutive = 0;
        }
    }
    
    if (consecutive < needed) return -1; // No space
    
    // Allocate program instructions
    for (int i = 0; i < program_len; i++) {
        mem->words[start + i].key = "instruction";
        mem->words[start + i].value = strdup(program[i]);
        mem->used[start + i] = 1;
    }
    
    // Allocate variables
    for (int i = 0; i < MAX_VARIABLES; i++) {
        mem->words[start + program_len + i].key = "variable";
        mem->words[start + program_len + i].value = NULL;
        mem->used[start + program_len + i] = 1;
    }
    
    // Store PCB fields
    pcb->mem_start = start;
    pcb->mem_end = start + needed - 1;
    
     // Actual PID set later
    char str[20]; 
    mem->words[pcb->mem_end - 5].key = "pid";
    snprintf(str,sizeof(str),"%d",pcb->pid);
    mem->words[pcb->mem_end - 5].value = strdup(str); // Actual PID set later
    mem->used[pcb->mem_end - 5] = 1;

    mem->words[pcb->mem_end - 4].key = "state";
    mem->words[pcb->mem_end - 4].value = (char*) ColorStrings[pcb->state];
    mem->used[pcb->mem_end - 4] = 1;

    mem->words[pcb->mem_end - 3].key = "priority";
    
    snprintf(str,sizeof(str), "%d", pcb->priority);
    mem->words[pcb->mem_end - 3].value = strdup(str);
    mem->used[pcb->mem_end - 3] = 1;

    mem->words[pcb->mem_end - 2].key = "program_counter";//object.
    pcb->program_counter = start;
    sprintf(str, "%d", pcb->program_counter);
    mem->words[pcb->mem_end - 2].value = strdup(str);
    mem->used[pcb->mem_end - 2] = 1;

    mem->words[pcb->mem_end - 1].key = "mem_start";
    sprintf(str, "%d", start);
    mem->words[pcb->mem_end - 1].value =strdup(str);
    mem->used[pcb->mem_end - 1] = 1;

    mem->words[pcb->mem_end].key = "mem_end";
    sprintf(str, "%d", start + needed - 1);
    mem->words[pcb->mem_end].value = strdup(str);
    mem->used[pcb->mem_end] = 1;

    

    // ... (store other PCB fields similarly)
    
    return start;
}

void increment_program_counter_mem(MemoryManager* mem, PCB* pcb){
    increment_program_counter(pcb);
    char str[20];
    sprintf(str, "%d", pcb->program_counter);
    mem->words[pcb->mem_end - 2].value = strdup(str);
}

void update_pcb_state_mem(MemoryManager* mem, PCB* pcb, ProcessState new_state){
    update_pcb_state(pcb, new_state);
    mem->words[pcb->mem_end - 4].value = (char*) ColorStrings[pcb->state];
}

void free_process(MemoryManager* mem, PCB* pcb) {
    for (int i = pcb->mem_start; i <= pcb->mem_end; i++) {
        // Free dynamically allocated value strings
        if (mem->words[i].value != NULL) {
            // free(mem->words[i].value);
            mem->words[i].value = NULL;
        }

        // Clear key and usage flag
        mem->words[i].key = NULL;
        mem->used[i] = 0;
    }

    // Optionally reset PCB memory bounds
    pcb->mem_start = -1;
    pcb->mem_end = -1;
}



void print_memory(MemoryManager* mem) {
    printf("Memory State:\n");
    
    // Loop through each memory word
    for (int i = 0; i < MEM_SIZE; i++) {
        // If the memory slot is in use
        if (mem->used[i]) {
            printf("Memory Slot %d: [USED] Key: %s, Value: %s\n", 
                   i, 
                   mem->words[i].key ? mem->words[i].key : "NULL", 
                   mem->words[i].value ? mem->words[i].value : "NULL");
        } else {
            // If the memory slot is free
            printf("Memory Slot %d: [FREE] Key: NULL, Value: NULL\n", i);
        }
    }
}

char* state_to_string(PCB* pcb){
    return (char*) ColorStrings[pcb->state];
}