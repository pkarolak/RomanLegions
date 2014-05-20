//<>< P.K.
#include <stdio.h>
#include <stdlib.h>
#include "vtime.h"

int main() {
	vtimer* v0 = Vtimer(3,0);
	vtimer* v1 = Vtimer(3,1);
	printf("before incrementation:\n");
	PrintVtimer(v0);
	for(int i = 0; i < 10 ; ++i) IncrementVtimer(v0);
	printf("after incrementation:\n");
	PrintVtimer(v0);
	PrintVtimer(v1);
	((v0->timer) > (v1->timer)) ? printf("wiekszy v0\n") : printf("wiekszy v1\n");
	SynchronizeVtimers(v1, v0);
	PrintVtimer(v1);
	((v0->timer) > (v1->timer)) ? printf("wiekszy v0\n") : printf("wiekszy v1\n");
	IncrementVtimer(v1);
	PrintVtimer(v1);
	((v0->timer) > (v1->timer)) ? printf("wiekszy v0\n") : printf("wiekszy v1\n");
	return 0;
}