//<>< P.K.
#ifndef _VTIME_H_
#define _VTIME_H_
#include <stdbool.h>

typedef struct vtimer {
	int size;
	int owner;
	int* timer;
} vtimer;

vtimer* Vtimer(int in_size, int in_owner);
int FreeVtimer(vtimer* v);
int CompareVtimers(vtimer* first, vtimer* second);
int IncrementVtimer(vtimer* v);
int SynchronizeVtimers(vtimer* local, vtimer* remote);
void PrintTimer(vtimer* v);
int max(int a, int b);

#endif