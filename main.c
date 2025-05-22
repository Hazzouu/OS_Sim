#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"  // Include scheduler functions
#include "memory.h"     // Include memory functions
#include "process.h" 
#include "interpreter.h"   // Include process functions
#include "queue.h"
#include "mutex.h"



char** separatefunction(char* fileName, int* line_count) {
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


int os_clock = 0;
int programs = 3;
Queue lvl1;
Queue lvl2;
Queue lvl3;
Queue lvl4;
Queue ready_queue;
Mutex file;
Mutex input;
Mutex output;
selected_schedule schedule;
int quanta;
int current_quanta;
PCB* pcbs_list[50];
char* filepaths[50];
int arr_index = 0 ;

int main(){

    quanta = 1;
    init_quanta();

    init_queue(&ready_queue);
    init_queue(&lvl1);
    init_queue(&lvl2);
    init_queue(&lvl3);
    init_queue(&lvl4);
    initMutex(&file);
    initMutex(&input);
    initMutex(&output);

    schedule = MLFQ;

    MemoryManager mem[60];
    init_memory(mem);

    
    pcbs_list[0] = create_pcb(1,7 );
    pcbs_list[1] = create_pcb(2,30);
    pcbs_list[2] = create_pcb(3,20);
    filepaths[0] = "/home/hasan/os_project/Program_1.txt";
    filepaths[1] = "Program_2.txt";
    filepaths[2] = "Program_3.txt";
    programs = arr_index = 3;


    
    
    print_memory(mem);

    // while(programs>0){
    //     multilevel_feedback_queue(mem, &lvl1, &lvl2,&lvl3,&lvl4);
    // }

    // while(programs>0){
    //     round_robin(mem,&ready_queue);
    // }

    for (size_t i = 0; i < 100; i++)
    {
        /* code */
    
        multilevel_feedback_queue(mem, &lvl1, &lvl2,&lvl3,&lvl4);
        // fifo_scheduler(mem, &ready_queue);
    }

    //fifo_scheduler(mem, &ready_queue);

    print_memory(mem);


       
    





    return 0;
}

