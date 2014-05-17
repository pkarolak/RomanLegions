#include "slave.h"
#include <time.h>
#include <stdlib.h>

char debug_message[1024];

int main() {
	int mytid;

	int master_id, my_num, nproc, legion_num, route_num, my_card;

	mytid = pvm_mytid();

	pvm_recv(-1, MSG_MSTR);
	pvm_upkint(&master_id, 1, 1);					// master id
	pvm_upkint(&my_num, 1, 1);						// index
	pvm_upkint(&nproc, 1, 1);						// number of processes
	pvm_upkint(&legion_num, 1, 1);					// number of legions
	pvm_upkint(&my_card, 1, 1);						// my_card
	pvm_upkint(&route_num, 1, 1);					// number of routes
	int* routes_capacity = calloc(route_num, sizeof(int));
	pvm_upkint(routes_capacity, route_num, 1);
	int* tids = calloc(nproc, sizeof(int));
	pvm_upkint(tids, nproc, 1);						// tids
	
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&my_num, 1, 1 );
	pvm_pkint(&mytid, 1, 1);
	pvm_send(master_id, MSG_SLV);

	//d_send(masterId, "Hello my master! I'm Legion with id: %d, %d, I know we have %d legions and %d routes", my_num, tids[myNum], legion_num, route_num);

	/*
		TODO:

	*/
	srand(time(NULL));
	msg core;
	core.sender_id = my_num;
	core.legion_card = my_card;
	core.timestamp = mytid;
	core.resource_id = rand()%route_num;
	for( int i = 0 ; i < my_num ; ++i ) {
		core.resource_id += rand()%route_num;
		core.resource_id %= route_num;
	}
	d_send(master_id, "sender: %d, card: %d, time: %d, res: %d\n", core.sender_id, core.legion_card, core.timestamp, core.resource_id);
	
	msg_send(master_id, MSG_DEBUG, core );
	free(tids);
	pvm_exit();
	return 0;
}

