#include "scheduler.h"
#include <stdio.h>
#include "shell.h"
#include "shellmemory.h"
#include <stdlib.h>
#include <string.h>
#include "pcb.h"


void schedule_no_sort (ready_queue * q, PCB ** pcbArray, int scripts) {
    int length = scripts;
    for (int i = 0; i < length; i++) {
        enqueue (q, pcbArray[i]);
    }

}
void schedule_sort (ready_queue * q, PCB ** pcbArray, int scripts) {
    qsort (pcbArray, scripts, sizeof (PCB *), compare_pcb_priority);
    for (int i = 0; i < scripts; i++) {
        // printf("i = %d\n", i);
        enqueue (q, pcbArray[i]);
    }
}
int execute_AGING (ready_queue * qAGING) {
    int timeout = 1;
    PCB *pcb;
    while ((pcb = dequeue (qAGING)) != NULL) {
        int script_id = pcb->process_index;
        int instruction = pcb->instruction;
        int lines = scriptmemory.scripts[script_id].lines;
        char **script_content = get_script_content (script_id);
        for (int i = instruction; i < timeout + instruction && i < lines; i++) {
            parseInput (script_content[i]);
            pcb->instruction++;
        }
        //Aging 
        AGE_PCBs (qAGING);
        //script not over
        if (pcb->instruction < lines) {
            PCB *current_head = qAGING->head;
            if (current_head != NULL) {
                if (pcb->priority <= current_head->priority) {
                    insert_front (qAGING, pcb);
                } else {
                    insert_sorted (qAGING, pcb);
                }
            } else {
                enqueue (qAGING, pcb);
            }
        } else {
            free (pcb);
            script_free_index (script_id);
        }
    }
}
void AGE_PCBs (ready_queue * qAGING) {
    PCB *pcb = qAGING->head;
    while (pcb != NULL) {
        if (pcb->priority > 0) {
            pcb->priority--;
        }
        pcb = pcb->next;
    }
}
int execute_RR (ready_queue * qRR, int timeout) {
    PCB *pcb;
    while ((pcb = dequeue (qRR)) != NULL) {
        int script_id = pcb->process_index;
        char **script_content = get_script_content (script_id);
        int lines = scriptmemory.scripts[script_id].lines;
        int instruction = pcb->instruction;
        for (int i = instruction; i < timeout + instruction && i < lines; i++) {
            parseInput (script_content[i]);
            pcb->instruction++;
        }
        // printf("instruction is : %d\n", pcb->instruction);
        // if there are still instructions left, re-queue the pcb
        if (pcb->instruction < lines) {
            enqueue (qRR, pcb);
        } else {
            free (pcb);
            script_free_index (script_id);
        }
    }
    return 0;
}

//Sort helper
int compare_pcb_priority (const void *a, const void *b) {
    PCB *pcbA = *(PCB **) a;
    PCB *pcbB = *(PCB **) b;
    return pcbA->priority - pcbB->priority;
}

// Non preemptive
int execute_non_preemptive (ready_queue * q) {
    PCB *pcb;
    while ((pcb = dequeue (q)) != NULL) {
        int script_id = pcb->process_index;
        // printf("script id: %d\n", script_id);
        int instruction = pcb->instruction;
        char **script_content = get_script_content (script_id);
        int lines = scriptmemory.scripts[script_id].lines;
        for (int i = instruction; i < lines; i++) {
            parseInput (script_content[i]);
        }
        script_free_index (script_id);
        free (pcb);
    }
    return 0;
}
enum POLICIES get_policy (char *policy_name) {
    if (strcmp (policy_name, "FCFS") == 0) {
        return FCFS;
    } else if (strcmp (policy_name, "SJF") == 0) {
        return SJF;
    } else if (strcmp (policy_name, "RR") == 0) {
        return RR;
    } else if (strcmp (policy_name, "AGING") == 0) {
        return AGING;
    } else if (strcmp (policy_name, "RR30") == 0) {
        return RR30;
    } else {
        return INVALID;
    }
}
