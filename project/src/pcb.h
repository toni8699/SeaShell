
#ifndef PCB_H
#   define PCB_H

/* 
    PCB  
*/
typedef struct PCB {
    int pid;
    int process_index;          // 0, 1, 2
    int instruction;            // current instruction index
    int priority;
    struct PCB *next;
    struct PCB *prev;
} PCB;

void pcb_init (PCB * pcb);
void pcb_free (PCB * pcb);
PCB **create_pcb_array (int scripts);


/// ready queue

typedef struct ready_queue {
    struct PCB *head;
    struct PCB *tail;
} ready_queue;

extern ready_queue readyQueue;


void queue_init (ready_queue * q);
void queue_free (ready_queue * q);
void enqueue (ready_queue * q, PCB * pcb);
PCB *dequeue (ready_queue * q);
void insert_front (ready_queue * q, PCB * pcb);
void insert_sorted (ready_queue * q, PCB * pcb);

void print_queue (ready_queue * q);


#endif // PCB_H
