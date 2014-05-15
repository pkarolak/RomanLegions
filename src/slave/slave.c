#include "slave.h"

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
	int* tids = calloc(nproc, nproc*sizeof(int));
	pvm_upkint(tids, 1, nproc);						// tids

	pvm_initsend(PvmDataDefault);
	pvm_pkint(&myNum, 1, 1 );
	pvm_pkint(&mytid, 1, 1);
	pvm_send(masterId, MSG_SLV);

	free(tids);
	pvm_exit();
	return 0;
}

