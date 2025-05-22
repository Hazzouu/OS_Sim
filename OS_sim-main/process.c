#include "process.h"
#include <stdlib.h>
#include <stdio.h>

PCB* create_pcb(int pid, int priority) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    pcb->pid = pid;
    pcb->state = NOT_IN_SYSTEM;
    pcb->priority = priority;
    pcb->program_counter = 0;
    pcb->mem_start = pcb->mem_end = -1;
    pcb->time_ready = pcb->time_running = pcb->time_blocked = 0;
    return pcb;
}


void destroy_pcb(PCB* pcb) {
    free(pcb);
}

void update_pcb_state(PCB* pcb, ProcessState new_state) {
    if (pcb->state != TERMINATED) {
        pcb->state = new_state;
    }
}

void increment_program_counter(PCB* pcb){
    pcb->program_counter++;
}

void print_pcb(PCB* pcb) {
    printf("PCB Details:\n");
    printf("PID: %d\n", pcb->pid);
    printf("State: %d\n", pcb->state);
    printf("Priority: %d\n", pcb->priority);
    printf("Program Counter: %d\n", pcb->program_counter);
    printf("Memory Allocation: %d - %d\n", pcb->mem_start, pcb->mem_end);
}