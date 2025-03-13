#include "pcb.h"
#include <stdlib.h>
#include <stdio.h>
#include "shellmemory.h"


// ready_queue readyQueue;
ready_queue readyQueue;
static int pid = 1;             // start at 1, leave 0 for the batch script.

void pcb_init (PCB * pcb) {
    pcb->pid = pid;
    pcb->instruction = 0;
    pcb->next = NULL;
    pcb->prev = NULL;
}
void pcb_free (PCB * pcb) {
    free (pcb);
}

PCB **create_pcb_array (int scripts) {
    PCB **pcbArray = malloc (sizeof (PCB) * scripts);
    // create a new PCB for each script
    for (int i = 0; i < scriptmemory.totalScripts; i++) {
        PCB *pcb = malloc (sizeof (PCB));
        pcb_init (pcb);
        pcb->priority = scriptmemory.scripts[i].lines;
        pcb->pid = pid;
        pcb->process_index = i;
        pcbArray[i] = pcb;
        pid++;
    }
    return pcbArray;
}

void enqueue (ready_queue * q, PCB * pcb) {
    pcb->next = NULL;           // New node's next is always NULL
    pcb->prev = q->tail;        // Set the new node's previous to the current tail

    if (q->tail != NULL) {      // if the queue is not empty
        q->tail->next = pcb;    // Link the current tail to the new node
    } else {
        q->head = pcb;          // If the queue is empty, set head to the new node
    }

    q->tail = pcb;              // Update the tail to the new node
}
PCB *dequeue (ready_queue * q) {
    if (q->head == NULL) {
        // printf("Queue is empty\n");
        return NULL;
    }
    PCB *pcb = q->head;
    // update the head and tail
    q->head = q->head->next;
    // if queueu not empty 
    if (q->head != NULL) {
        // new head prev points to null
        q->head->prev = NULL;
    } else {
        // if queue is empty tail also points to null
        q->tail = NULL;
    }
    return pcb;
}
void insert_sorted (ready_queue * q, PCB * pcb) {
    PCB *current = q->head;
    //If queue is empty
    if (current == NULL) {
        enqueue (q, pcb);
    } else {
        while (current != NULL && pcb->priority >= current->priority) {
            current = current->next;
        }
        //End of list, enqueue
        if (current == NULL) {
            enqueue (q, pcb);
        } else {
            pcb->next = current;
            pcb->prev = current->prev;
            if (current == q->head) {
                q->head = pcb;
            } else {
                current->prev->next = pcb;
            }
            current->prev = pcb;
        }
    }
}

// Oldhead -> a ->taik
/// pcb -> oldhead - > a ->taik

void insert_front (ready_queue * q, PCB * pcb) {
    if (q->head == NULL) {
        enqueue (q, pcb);
    } else {
        pcb->next = q->head;
        q->head->prev = pcb;
        q->head = pcb;
        // pcb->prev = NULL;
    }
}




void print_queue (ready_queue * q) {
    PCB *current = q->head;
    int count = 0;
    while (current != NULL) {
        printf ("script index is %d", current->process_index);
        printf ("->");
        current = current->next;
        count++;
    }
    printf ("\n");
    // printf("there are %d processes in the queue\n", count);
}
void queue_init (ready_queue * q) {
    q->head = NULL;
    q->tail = NULL;
}
void queue_free (ready_queue * q) {
    PCB *current = q->head;
    while (current != NULL) {
        PCB *next = current->next;
        free (current);
        current = next;
    }
    q->head = NULL;
    q->tail = NULL;
}
