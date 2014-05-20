//<>< P.K.
#include <stdlib.h>
#include <time.h>
#include "slave.h"
#include "queue.h"

char debug_message[1024];
char debug_queue[1024];
int RequestedResourceId = -1;
int NonConfirmedRequest[1024];
int ToConfirm = 0;
vtimer* MyTimer;
int MasterId, MyNum, nproc, LegionNum, RouteNum, MyCard;
int MyTid;
queue** ResourceQueue;
int* RoutesCapacity;
int* tids;

int CriticalSection() {
	SendComunicate(MasterId, "\t\t\t\t>%d entered CS on route %d\n", MyNum, RequestedResourceId);
	message msg;
	double tstart, cs;
	cs = 0;
	tstart = (double)clock()/CLOCKS_PER_SEC;
	while(cs < 3) {
		if(NonBlockingReceiveMessage(&msg, REQUEST)) {
			IncrementVtimer(MyTimer);
			SynchronizeVtimers(MyTimer, &(msg.timer));
			QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.legion_card, msg.sender_id)));
			IncrementVtimer(MyTimer);
			message* respond = Message(MyNum, MyCard, *MyTimer, msg.resource_id);
			SendMessage(tids[msg.sender_id], respond, ACK);
			FreeMessage(respond);
		}
		if(NonBlockingReceiveMessage(&msg, FREE)) {
			IncrementVtimer(MyTimer);
			QueueDeleteIndex(ResourceQueue[msg.resource_id], msg.sender_id);
		}
		cs = (double)clock()/CLOCKS_PER_SEC - tstart;
	}
	SendComunicate(MasterId, "\t\t\t\t<%d left CS on route %d\n", MyNum, RequestedResourceId);
	FreeResource(RequestedResourceId);
	return 0;
}

int ResourceRequest(int ResourceId) {
	//SendComunicate(MasterId, "Legion %d will send resource request on Route %d\n", MyNum, ResourceId);
	for(int i = 0 ; i < nproc ; ++i) {
		NonConfirmedRequest[i] = 1;
	}
	ToConfirm = nproc;
	QueuePush(ResourceQueue[ResourceId], Node(Pair(MyCard, MyNum)));
	RequestedResourceId = ResourceId;
	IncrementVtimer(MyTimer);
	message* msg = Message(MyNum, MyCard, *MyTimer, ResourceId);
	for(int i = 0 ; i < nproc ; ++i) {
		if(i != MyNum) {
			SendMessage(tids[i], msg, REQUEST);
		}
	}
	FreeMessage(msg);
	//SendComunicate(MasterId, "\t>Legion %d sent resource request on Route %d\n", MyNum, ResourceId);
	return 0;
}

int FreeResource(int ResourceId) {
	//SendComunicate(MasterId, "Legion %d will release Route %d\n", MyNum, ResourceId);
	RequestedResourceId = -1;
	ToConfirm = -1;
	QueueDeleteIndex(ResourceQueue[ResourceId], MyNum);
	IncrementVtimer(MyTimer);
	message* msg = Message(MyNum, MyCard, *MyTimer, ResourceId);
	for(int i = 0 ; i < nproc ; ++i) {
		if(i != MyNum) {
			SendMessage(tids[i], msg, FREE);
		}
	}
	FreeMessage(msg);
	//SendComunicate(MasterId, "\t\t\t>Legion %d has realeased Route %d\n", MyNum, ResourceId);
	return 0;
}

int ReceiveMessageRoutines() {
	message msg;
	if(NonBlockingReceiveMessage(&msg, REQUEST)) {
		//SendComunicate(MasterId, "\t\t>Legion %d received REQUEST from %d on resource %d\n", MyNum, msg.sender_id, msg.resource_id);
		IncrementVtimer(MyTimer);
		if ( (msg.resource_id == RequestedResourceId) && ( NonConfirmedRequest[msg.sender_id] == 1 ) ) {
			int cmp = CompareVtimers(&(msg.timer), MyTimer);
			if( (cmp == -1) || ((cmp == 0) && (msg.sender_id < MyNum)) ) {
				QueuePushBeforeOwner(ResourceQueue[msg.resource_id], Node(Pair(msg.legion_card, msg.sender_id)));
				SendComunicate(MasterId, "\t\t<Legion %d let %d go before on route %d\n", MyNum, msg.sender_id, msg.resource_id);
			}
			else {
				QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.legion_card, msg.sender_id)));
				SendComunicate(MasterId, "\t\t<Legion %d has won conflict with %d on route %d\n", MyNum, msg.sender_id, msg.resource_id);
			}
			QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.legion_card, msg.sender_id)));
			NonConfirmedRequest[msg.sender_id] = 0;
			--ToConfirm;
			if( (ToConfirm == 1) && ((PredecessorsCard(ResourceQueue[RequestedResourceId]) + MyCard) <= RoutesCapacity[RequestedResourceId]) ) {			
				CriticalSection();
			}
		}
		else {
			IncrementVtimer(MyTimer);
			QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.legion_card, msg.sender_id)));
			message* respond = Message(MyNum, MyCard, *MyTimer, msg.resource_id);
			SendMessage(tids[msg.sender_id], respond, ACK);
			FreeMessage(respond);
		}
		SynchronizeVtimers(MyTimer, &(msg.timer));
	}
	if(NonBlockingReceiveMessage(&msg, FREE)) {
		//SendComunicate(MasterId, "\t\t>Legion %d received FREE from %d on resource %d\n", MyNum, msg.sender_id, msg.resource_id);
		IncrementVtimer(MyTimer);
		QueueDeleteIndex(ResourceQueue[msg.resource_id], msg.sender_id);
		if(msg.resource_id == RequestedResourceId) {
			//SendComunicate(MasterId, "\t\t#Legion %d is recalculating entrance on Route %d\n", MyNum, msg.resource_id);	
			if( (PredecessorsCard(ResourceQueue[RequestedResourceId]) + MyCard) <= RoutesCapacity[RequestedResourceId] ) {			
				CriticalSection();
			}	
		}
	}
	if(NonBlockingReceiveMessage(&msg, ACK)) {
		SendComunicate(MasterId, "\t\t>Legion %d received ACK from %d on resource %d\n", MyNum, msg.sender_id, msg.resource_id);
		IncrementVtimer(MyTimer);
		--ToConfirm;
		if( (ToConfirm == 1) && ((PredecessorsCard(ResourceQueue[RequestedResourceId]) + MyCard) <= RoutesCapacity[RequestedResourceId]) ) {			
			CriticalSection();
		}
	}
	return 0;
}


int main() {
	srand(time(NULL));
	MyTid = pvm_mytid();
	pvm_recv(-1, MASTER);
	pvm_upkint(&MasterId, 1, 1);					// master id
	pvm_upkint(&MyNum, 1, 1);						// index
	pvm_upkint(&nproc, 1, 1);						// number of processes
	pvm_upkint(&LegionNum, 1, 1);					// number of legions
	pvm_upkint(&MyCard, 1, 1);						// MyCard
	pvm_upkint(&RouteNum, 1, 1);					// number of routes
	RoutesCapacity = calloc(RouteNum, sizeof(int));
	pvm_upkint(RoutesCapacity, RouteNum, 1);
	tids = calloc(nproc, sizeof(int));
	pvm_upkint(tids, nproc, 1);						// tids
	MyTimer = Vtimer(LegionNum, MyNum);
	ResourceQueue = calloc(RouteNum, sizeof(queue*));
	for(int i = 0; i < RouteNum ; ++i) {
		ResourceQueue[i] = Queue(MyNum);
	}
	//IncrementVtimer(my_timer);
	
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&MyNum, 1, 1 );
	pvm_pkint(&MyTid, 1, 1);
	pvm_send(MasterId, SLAVE);

	srand(time(NULL));
	int resource_id = rand()%RouteNum;
	for( int i = 0 ; i < MyNum ; ++i ) {
		resource_id += rand()%RouteNum;
		resource_id %= RouteNum;
	}
	message* core = Message(MyTid, MyCard, *MyTimer, resource_id);
	SendMessage(MasterId, core, COMMUNICATE);

	while(1) {
		int enter = rand()%2;
		if((RequestedResourceId == -1) && enter) {
			ResourceRequest(rand()%RouteNum);
			SendComunicate(MasterId, "%d wants to enter CS on route %d\n", MyNum, RequestedResourceId);
		}
		ReceiveMessageRoutines();
		IncrementVtimer(MyTimer);
	}

	for (int i = 0; i < RouteNum; ++i) {
		FreeQueue(ResourceQueue[i]);
	}
	free(ResourceQueue);
	FreeVtimer(MyTimer);
	free(tids);
	free(RoutesCapacity);
	pvm_exit();
	return 0;
}

