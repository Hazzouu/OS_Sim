// queue.h
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "process.h"

#define MAX_SIZE 100



typedef struct Queue {
    PCB* processes[100];
    int front;
    int rear;
} Queue;

void init_queue(Queue* q);
void enqueue(Queue* q, PCB* process);
PCB* dequeue(Queue* q);
PCB* peek(Queue* q);
int is_empty(Queue* q);
void print_queue(Queue* q);
void clear_queue(Queue* q);

#endif // QUEUE_H
