// C Program to demonstrate how to Implement a queue
#include <stdbool.h>
#include <stdio.h>
#include "queue.h"
#include "process.h"
#define MAX_SIZE 100

void init_queue(Queue* q) {
    q->front = 0;
    q->rear = 0;
}

void enqueue(Queue* q, PCB* process) {
    q->processes[q->rear++] = process;
}

PCB* dequeue(Queue* q) {
    if (q->front == q->rear) {
        return NULL;
    }
    return q->processes[q->front++];
}

PCB* peek(Queue* q) {
    if (q->front == q->rear) {
        return NULL;
    }
    return q->processes[q->front];
}

int is_empty(Queue* q) {
    return q->front == q->rear;
}

void print_queue(Queue* q) {
    if (is_empty(q)) {
        printf("Queue is empty.\n");
        return;
    }

    printf("Queue contents:\n");
    for (int i = q->front; i < q->rear; i++) {
        PCB* process = q->processes[i];
        printf("Process ID: %d, State: %d, Priority: %d, Program Counter: %d\n",
               process->pid, process->state, process->priority, process->program_counter);
    }
}

void clear_queue(Queue* q) {
    for (int i = 0; i < q->rear; i++) {
        // If you dynamically allocated PCBs and want to free them:
        // free(q->processes[i]);
        q->processes[i] = NULL;
    }
    q->front = 0;
    q->rear = 0;
}