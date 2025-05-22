#ifndef PROCESS_H
#define PROCESS_H


typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED,
    NOT_IN_SYSTEM
} ProcessState;



typedef struct {
    int pid;                // Process ID
    ProcessState state;     // Current state
    int priority;           // Priority (1-4)
    int program_counter;    // Next instruction index
    int mem_start;          // Memory start address
    int mem_end;            // Memory end address
    int time_ready;
    int time_running;
    int time_blocked;
} PCB;

PCB* create_pcb(int pid, int priority);
void destroy_pcb(PCB* pcb);
void update_pcb_state(PCB* pcb, ProcessState new_state);
void increment_program_counter(PCB* pcb);
void print_pcb(PCB* pcb);

#endif