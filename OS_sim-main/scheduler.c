#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "process.h"
#include "memory.h"
#include "interpreter.h"
#include "queue.h"
#include "string.h"
#include "gui.h"

extern AppWidgets *app; // Access global AppWidgets





static char** separatefunction(char* fileName, int* line_count) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    char ch;
    char result[100] = "";
    char** lines = malloc(sizeof(char*) * 100); // support up to 100 lines
    int i = 0;

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines[i++] = strdup(result); // save the current line
            result[0] = '\0';            // clear buffer
        } else {
            char temp[2] = {ch, '\0'};
            strcat(result, temp);        // build line char by char
        }
    }

    // Handle last line if file doesn’t end with newline
    if (strlen(result) > 0) {
        lines[i++] = strdup(result);
    }

    fclose(file);
    *line_count = i;
    return lines;
}

static PCB* check_blocked(){
    while(peek(&ready_queue)->state == BLOCKED){
        dequeue(&ready_queue);
        if(is_empty(&ready_queue)){
            return NULL;
        }
    }
    return peek(&ready_queue);
}

static void arrivals(MemoryManager* mem) {
    for (int i = 0; i < programs; i++) {
        if (pcbs_list[i] != NULL && os_clock == pcbs_list[i]->priority) {
            int line_count;
            char** lines = separatefunction(filepaths[i], &line_count);
            allocate_process(mem, pcbs_list[i], lines, line_count);
            update_pcb_state_mem(mem, pcbs_list[i], READY);
            enqueue(&ready_queue, pcbs_list[i]);
            printf("Process ID %d has arrived.\n", pcbs_list[i]->pid);
        }
    }
}

void fifo_scheduler(MemoryManager* memory, Queue* ready_queue) {
    // Check for process arrivals
    printf("os_clock: %d\n", os_clock);

    arrivals(memory);
    // If no processes are ready, advance the os_clock
    if (is_empty(ready_queue)) {
        os_clock++;
        return;
    }  
    

    PCB* current_process = peek(ready_queue);

    if(current_process->state != RUNNING){
        update_pcb_state_mem(memory,current_process,RUNNING);
    }

    if (current_process == NULL){
        os_clock ++;
        return;
    }

    

    // Check if the process has remaining instructions
    if (current_process->program_counter < current_process->mem_end - 8) {
        printf("Executing Process ID: %d\n", current_process->pid);
        execute_instruction(memory, current_process);
        
        if(current_process->program_counter >= current_process->mem_end - 8){
            printf("Process ID %d completed.\n", current_process->pid);
            update_pcb_state_mem(memory,current_process,TERMINATED);
            free_process(memory,current_process);
            dequeue(ready_queue);
        }
    } 
}


void init_quanta(){
    current_quanta = quanta;
}

void round_robin(MemoryManager* mem , Queue* ready_queue){
    
    printf("os_clock: %d\n", os_clock);


    arrivals(mem);
    
    
    if(is_empty(ready_queue)){
        os_clock++;
        return;
    }
    

    if(current_quanta == 0){
        current_quanta = quanta;
        PCB* temp = dequeue(ready_queue);
        if(temp->state != BLOCKED){
            update_pcb_state_mem(mem,temp,READY);
            enqueue(ready_queue,temp);
        }
        
    }

    PCB* current_process = peek(ready_queue);
    

    if(current_process->state == BLOCKED){
        current_quanta = quanta;
        current_process = check_blocked();
        if (current_process ==NULL){
            os_clock ++;
            return;
        }
    }

    
    
    

    if(current_process->program_counter < current_process->mem_end - 8){
        printf("Executing Process ID: %d\n", current_process->pid);
        update_pcb_state_mem(mem,current_process,RUNNING);
        execute_instruction(mem,current_process); // executing
        current_quanta--;
        if(current_process->program_counter >= current_process->mem_end - 8){
            current_quanta = quanta;
            update_pcb_state_mem(mem,current_process,TERMINATED);
            free_process(mem,current_process);
            printf("Process ID %d completed.\n", current_process->pid);
            dequeue(ready_queue);
            
        }
    }
}

const int quantum[] = {1, 2, 4, 8}; // Quantum for each level
int cur_quantum[] = {1, 2, 4, 8};
bool new_arrival = false;
Queue* current_queue = NULL;  // Initialize to NULL at global scope
Queue* next_queue = NULL;

void multilevel_feedback_queue(MemoryManager* mem, Queue* level1, Queue* level2, Queue* level3, Queue* level4) {
    // Check for process arrivals first
    for (int i = 0; i < programs; i++) {
        if (pcbs_list[i] != NULL && os_clock == pcbs_list[i]->priority) {
            int line_count;
            char** lines = separatefunction(filepaths[i], &line_count);
            allocate_process(mem, pcbs_list[i], lines, line_count);
            update_pcb_state_mem(mem, pcbs_list[i], READY);
            enqueue(level1, pcbs_list[i]);
            printf("Process ID %d has arrived.\n", pcbs_list[i]->pid);
            new_arrival = true;
        }
    }

    if (!is_empty(level1) || !is_empty(level2) || !is_empty(level3) || !is_empty(level4) || programs > 0) {
        
        // Process each level in order of priority
        if (!is_empty(level1) && !all_blocked(level1)) {
            if (new_arrival && (current_queue != level1 && current_queue != NULL)) {
                // Finish current quantum before potentially switching
                if (current_queue == level2) {
                    execute_level(mem, level2, level3, 1);
                } else if (current_queue == level3) {
                    execute_level(mem, level3, level4, 2);
                } else if (current_queue == level4) {
                    execute_level(mem, level4, level4, 3);
                }
            } else {
                current_queue = level1; // Set current queue before executing
                next_queue = level2;
                execute_level(mem, level1, level2, 0);
            }
        } else if (!is_empty(level2) && !all_blocked(level2)) {
            if (current_queue == level2) {
                execute_level(mem, level2, level3, 1);
                return;
            } else if (current_queue == level3) {
                execute_level(mem, level3, level4, 2);
                return;
            } else if (current_queue == level4) {
                execute_level(mem, level4, level4, 3);
                return;
            }
            current_queue = level2;
            execute_level(mem, level2, level3, 1);
        } else if (!is_empty(level3)&& !all_blocked(level3)) {
            if (current_queue == level2) {
                execute_level(mem, level2, level3, 1);
                return;
            } else if (current_queue == level3) {
                execute_level(mem, level3, level4, 2);
                return;
            } else if (current_queue == level4) {
                execute_level(mem, level4, level4, 3);
                return;
            }
            current_queue = level3;
            execute_level(mem, level3, level4, 2);
        } else if (!is_empty(level4) && !all_blocked(level4)) {
            if (current_queue == level2) {
                execute_level(mem, level2, level3, 1);
                return;
            } else if (current_queue == level3) {
                execute_level(mem, level3, level4, 2);
                return;
            } else if (current_queue == level4) {
                execute_level(mem, level4, level4, 3);
                return;
            }
            current_queue = level4;
            execute_level(mem, level4, level4, 3);
        } else {
            current_queue = NULL; // No queue active
            os_clock++; // Advance os_clock if no processes are ready
        }
    }
}

void execute_level(MemoryManager* mem, Queue* current_level, Queue* next_level, int quantum_index) {
    PCB* current_process = peek(current_level);

    while(current_process->state == BLOCKED){
        PCB* temp = dequeue(current_level);
        enqueue(current_level,temp);
        current_process = peek(current_level);
    }

    printf("Executing Process ID: %d at Level with Quantum %d\n", current_process->pid, quantum[quantum_index]);
    if (current_process->state != RUNNING){
        update_pcb_state_mem(mem, current_process, RUNNING);
    }
    
    execute_instruction(mem, current_process);
    update(mem);
    // if(current_process->state==BLOCKED){
    //     PCB* temp = dequeue(current_level);
    //     enqueue(current_level,temp);
    //     current_queue = NULL;
    //     cur_quantum[quantum_index] = quantum[quantum_index];
    //     return;
    // }
    cur_quantum[quantum_index]--;
    
    // Check if the process has completed execution
    if (current_process->program_counter < current_process->mem_end - 8) {
        if(cur_quantum[quantum_index] == 0) {
            current_queue = NULL;
            cur_quantum[quantum_index] = quantum[quantum_index];
            if(current_process->state !=BLOCKED){
                update_pcb_state_mem(mem, current_process, READY);
            }
            dequeue(current_level);
            if (current_level != next_level) {
                printf("Process ID %d moved to the next level.\n", current_process->pid);
                enqueue(next_level, current_process);
            } else {
                enqueue(current_level, current_process);
            }
            new_arrival = false;
        }else if(current_process->state==BLOCKED){
            PCB* temp = dequeue(current_level);
            enqueue(current_level,temp);
            current_queue = NULL;
            cur_quantum[quantum_index] = quantum[quantum_index];
        }else{
            current_queue = current_level;
        }
    } else {
        dequeue(current_level);
        cur_quantum[quantum_index] = quantum[quantum_index];
        update_pcb_state_mem(mem, current_process, TERMINATED);
        free_process(mem, current_process);
        printf("Process ID %d completed.\n", current_process->pid);
        new_arrival = false;
        current_queue = NULL;
    }
    printf("%d \n", cur_quantum[quantum_index]);
}

int all_blocked(Queue* q){
    for (int i = q->front; i < q->rear; i++) {
        if (q->processes[i]->state != BLOCKED) {
            return 0; // Found a process that is not blocked
        }
    }
    return 1; // All processes are blocked
}


 void update(MemoryManager* mem) {

    char cycle_log[256];

    for (int i = 0; i < programs; i++) {
        if (pcbs_list[i] != NULL && pcbs_list[i]->state == RUNNING && pcbs_list[i]->mem_start != -1) {
            int prev_pc = pcbs_list[i]->program_counter - 1;
            if (prev_pc >= pcbs_list[i]->mem_start && prev_pc < pcbs_list[i]->mem_end &&
                prev_pc >= 0 && prev_pc < MEM_SIZE) {
                char *instruction = mem[0].words[prev_pc].value;
                if (instruction) {
                    char log[256];
                    snprintf(log, sizeof(log), "P%d executed instruction: %s\n",
                             pcbs_list[i]->pid, instruction);
                    gtk_text_buffer_insert_at_cursor(
                        gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->log_text_view)),
                        log, -1);
                }
            }
            break;
        }
    }

    // Update process table
    if (app->process_store) {
        gtk_list_store_clear(app->process_store);
        for (int i = 0; i < programs; i++) {
            if (pcbs_list[i] && pcbs_list[i]->state >= READY && pcbs_list[i]->state <= NOT_IN_SYSTEM) {
                char pid_str[16];
                snprintf(pid_str, sizeof(pid_str), "P%d", pcbs_list[i]->pid);
                char range_str[50];
                if (pcbs_list[i]->mem_start >= 0 && pcbs_list[i]->mem_end < MEM_SIZE &&
                    pcbs_list[i]->mem_start <= pcbs_list[i]->mem_end) {
                    snprintf(range_str, sizeof(range_str), "%d-%d", pcbs_list[i]->mem_start, pcbs_list[i]->mem_end);
                } else {
                    snprintf(range_str, sizeof(range_str), "N/A");
                }
                GtkTreeIter iter;
                gtk_list_store_append(app->process_store, &iter);
                gtk_list_store_set(app->process_store, &iter,
                                   0, pid_str,
                                   1, state_to_string(pcbs_list[i]),
                                   2, pcbs_list[i]->priority,
                                   3, range_str,
                                   4, pcbs_list[i]->program_counter,
                                   5, pcbs_list[i]->priority,
                                   -1);
            }
        }
    }

    update_memory_view(app);
    update_gui(app);
    update_ready_queue_label(app);
    update_running_and_blocked_labels(app);
    update_mutex_status_label(app);
    update_blocked_labels(app);
    update_current_instruction_label(app);

    snprintf(cycle_log, sizeof(cycle_log), "Cycle %d: Running: ", app->clock_cycle);
    int pos = strlen(cycle_log);
    int running_found = 0;
    for (int i = 0; i < programs; i++) {
        if (pcbs_list[i] != NULL && pcbs_list[i]->state == RUNNING) {
            pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, "P%d", pcbs_list[i]->pid);
            running_found = 1;
            break;
        }
    }
    if (!running_found) {
        pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, "None");
    }
    pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, ", Ready: [");
    if (!is_empty(&ready_queue)) {
        int first = 1;
        for (int i = ready_queue.front; i < ready_queue.rear && i < MAX_SIZE; i++) {
            if (ready_queue.processes[i] == NULL || ready_queue.processes[i]->state != READY) continue;
            if (!first) pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, ", ");
            pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, "P%d", ready_queue.processes[i]->pid);
            first = 0;
        }
    }
    pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, "], Blocked: [");
    int first = 1;
    for (int i = 0; i < programs; i++) {
        if (pcbs_list[i] != NULL && pcbs_list[i]->state == BLOCKED) {
            if (!first) pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, ", ");
            pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, "P%d", pcbs_list[i]->pid);
            first = 0;
        }
    }
    pos += snprintf(cycle_log + pos, sizeof(cycle_log) - pos, "]\n");
    gtk_text_buffer_insert_at_cursor(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->log_text_view)),
        cycle_log, -1);

    snprintf(cycle_log, sizeof(cycle_log), "Clock cycle %d executed\n", app->clock_cycle);
    gtk_text_buffer_insert_at_cursor(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->log_text_view)),
        cycle_log, -1);
}