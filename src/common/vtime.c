//<>< P.K.
#include <stdlib.h>
#include <stdio.h>
#include "vtime.h"

vtimer* Vtimer(int in_size, int in_owner) {
	vtimer* v = calloc(1, sizeof(vtimer));
	v->size = in_size;
	v->owner = in_owner;
	v->timer = 0;
	/*for(int i = 0 ; i < MAX_TIMER_SIZE ; ++i) {
		v->timer[i] = 0;
	}*/
	return v;
}

int FreeVtimer(vtimer* v) {
	free(v);
	return 0;
}

/*int CompareVtimers(vtimer* first, vtimer* second) {
	if(first->timer[first->owner] == second->timer[second->owner]) {
		return 0; // f == s
	}
	if(first->timer[first->owner] > second->timer[second->owner]) {
		return 1; // f > s
	}
	if(first->timer[first->owner] < second->timer[second->owner]) {
		return -1; // f < s
	}
	return -2; 
	if( first->size != second->size ) {
		return -2;
	}
	int carry_on = 1;
	for(int i = 0 ; (i < first->size) && (carry_on == 1); ++i) {
		if( first->timer[i] != second->timer[i] ) {
			carry_on = 0;
			break;
		}
	}
	if(carry_on == 1) {
		return 0; // f == s
	}
	carry_on = 1;
	for(int i = 0 ; (i < first->size) && (carry_on == 1); ++i) {
		if( first->timer[i] < second->timer[i] ) {
			carry_on = 0;
			break;
		}
	}
	if(carry_on == 1) {
		return 1; // f > s
	}
	carry_on = 1;
	for(int i = 0 ; (i < first->size) && (carry_on == 1); ++i) {
		if( first->timer[i] > second->timer[i] ) {
			carry_on = 0;
			break;
		}
	}
	if(carry_on == 1) {
		return -1; // f < s
	}
	return -2;
}
	*/

int IncrementVtimer(vtimer* v) {
	++(v->timer);
	//++(v->timer[v->owner]);
	return 0;
}

int SynchronizeVtimers(vtimer* local, vtimer* remote) {
	/*if( local->size != remote->size ) {
		return -1;
	}
	for(int i = 0 ; i < local->size ; ++i) {
		local->timer[i] = max(local->timer[i], remote->timer[i]);
	}*/
	local->timer = max(local->timer, remote->timer);
	return 0;
}

int max(int a, int b) {
	return (a > b) ? a : b;
}


void PrintVtimer(vtimer* v) {
	printf("\nTimer of: %d: %d\n", v->owner, v->timer);
	/*
	printf("[ ");
	for(int i = 0 ; i < v->size; ++i) {
		if ( i == v->owner ) {
			printf("_%d_ ", v->timer[i]);
		}
		else {
			printf("%d ", v->timer[i]);
		}
	}
	printf("]\n");
	*/
}
int GetOwnerVtime(vtimer* v) {
	//return(v->timer[v->owner]);
	return(v->timer);
}