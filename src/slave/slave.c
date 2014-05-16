#include "slave.h"

char debug_message[1024];

int main() {
	int mytid;

	int masterId, myNum, nproc, legion_num, route_num;

	mytid = pvm_mytid();

	pvm_recv(-1, MSG_MSTR);
	pvm_upkint(&masterId, 1, 1);					// master id
	pvm_upkint(&myNum, 1, 1);						// index
	pvm_upkint(&nproc, 1, 1);						// number of processes
	pvm_upkint(&legion_num, 1, 1);					// number of legions
	pvm_upkint(&route_num, 1, 1);					// number of routes
	int* tids = calloc(nproc, sizeof(int));
	pvm_upkint(tids, nproc, 1);						// tids
	
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&myNum, 1, 1 );
	pvm_pkint(&mytid, 1, 1);
	pvm_send(masterId, MSG_SLV);

	d_send(masterId, "Hello my master! I'm Legion with id: %d, %d, I know we have %d legions and %d routes", myNum, tids[myNum], legion_num, route_num);

	/*
		TODO:

	*/
		
	free(tids);
	pvm_exit();
	return 0;
}

