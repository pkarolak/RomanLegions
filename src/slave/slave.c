//<>< P.K.
#include <stdlib.h>
#include <time.h>
#include "slave.h"
#include "queue.h"


char debug_message[1024];

int main() {
	int mytid;

	int master_id, my_num, nproc, legion_num, route_num, my_card;

	mytid = pvm_mytid();
	pvm_recv(-1, MASTER);
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

	vtimer* my_timer = Vtimer(legion_num, my_num);
	//IncrementVtimer(my_timer);
	
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&my_num, 1, 1 );
	pvm_pkint(&mytid, 1, 1);
	pvm_send(master_id, SLAVE);

	//d_send(masterId, "Hello my master! I'm Legion with id: %d, %d, I know we have %d legions and %d routes", my_num, tids[myNum], legion_num, route_num);

	srand(time(NULL));
	int resource_id = rand()%route_num;
	for( int i = 0 ; i < my_num ; ++i ) {
		resource_id += rand()%route_num;
		resource_id %= route_num;
	}
	message* core = Message(mytid, my_card, *my_timer, resource_id);
	SendMessage(master_id, core, COMMUNICATE);
	FreeVtimer(my_timer);
	free(tids);
	pvm_exit();
	return 0;
}

