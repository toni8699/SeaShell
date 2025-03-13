#include "pcb.h"

// #define POLICIES { FCFS, SJF, RR }
enum POLICIES { FCFS, SJF, RR, AGING, RR30, INVALID };

void schedule_no_sort (ready_queue * qFIFO, PCB ** pcbArray, int scripts);
void schedule_sort (ready_queue * qSJF, PCB ** pcbArray, int scripts);


int execute_non_preemptive (ready_queue * q);
int execute_RR (ready_queue * q, int timeout);

int execute_AGING (ready_queue * qAGING);

enum POLICIES get_policy (char *policy_name);


int compare_pcb_priority (const void *a, const void *b);
void AGE_PCBs (ready_queue * qAGING);
