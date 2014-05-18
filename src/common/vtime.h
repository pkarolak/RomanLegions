//<>< P.K.
#ifndef _VTIME_H_
#define _VTIME_H_
#include <stdbool.h>
#define MAX_TIMER_SIZE 1024

typedef struct vtimer {
	int size;
	int owner;
	int timer[MAX_TIMER_SIZE];
} vtimer;

vtimer* Vtimer(int in_size, int in_owner);
int FreeVtimer(vtimer* v);
int CompareVtimers(vtimer* first, vtimer* second);
int IncrementVtimer(vtimer* v);
int SynchronizeVtimers(vtimer* local, vtimer* remote);
void PrintVtimer(vtimer* v);
int max(int a, int b);

#endif