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
int WaitingForResource = 0;
int ScalarTimer = 0;
int Timestamp = 0;

int CriticalSection() {
	WaitingForResource = 0;
	SendComunicate(MasterId, "%d\t\t\t\t[CS]>> %d entered CS on route %d\n", GetOwnerVtime(MyTimer), MyNum, RequestedResourceId);
	message msg;
	double tstart, cs;
	cs = 0;
	tstart = (double)clock()/CLOCKS_PER_SEC;
	while(cs < 1) {
		if(NonBlockingReceiveMessage(&msg, REQUEST)) {
			SynchronizeVtimers(MyTimer, &(msg.timer));
			IncrementVtimer(MyTimer);
			QueueDeleteIndex(ResourceQueue[msg.resource_id], msg.sender_id);
			QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.sender_id, msg.legion_card)));
			IncrementVtimer(MyTimer);
			SendComunicate(MasterId, "%d\t\t[R]>> Legion %d received REQUES from %d on resource %d\n", GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
			message* respond = Message(MyNum, MyCard, *MyTimer, msg.resource_id);
			SendMessage(tids[msg.sender_id], respond, ACK);
			SendComunicate(MasterId, "%d\t\t[A]<< Legion %d respodned %d on route %d with simple ACK\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
			FreeMessage(respond);
		}
		if(NonBlockingReceiveMessage(&msg, FREE)) {
			SynchronizeVtimers(MyTimer, &(msg.timer));
			IncrementVtimer(MyTimer);
			SendComunicate(MasterId, "%d\t\t[F]>> Legion %d received FREE from %d on resource %d\n", GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
			QueueDeleteIndex(ResourceQueue[msg.resource_id], msg.sender_id);
		}
		cs = (double)clock()/CLOCKS_PER_SEC - tstart;
	}
	FreeResource(RequestedResourceId);
	return 0;
}

int ResourceRequest(int ResourceId) {
	WaitingForResource = 1;
	SendComunicate(MasterId, "%d\t[i] Legion %d will send resource request on Route %d\n", GetOwnerVtime(MyTimer), MyNum, ResourceId);
	for(int i = 0 ; i < nproc ; ++i) {
		if(i != MyNum) {
			NonConfirmedRequest[i] = 1;
		}
	}
	ToConfirm = nproc;
	QueuePush(ResourceQueue[ResourceId], Node(Pair(MyNum, MyCard)));
	RequestedResourceId = ResourceId;
	IncrementVtimer(MyTimer);
	Timestamp = GetOwnerVtime(MyTimer);
	message* msg = Message(MyNum, MyCard, *MyTimer, ResourceId);
	for(int i = 0 ; i < nproc ; ++i) {
		if(i != MyNum) {
			SendMessage(tids[i], msg, REQUEST);
		}
	}
	SendComunicate(MasterId, "%d\t[i] Legion %d sent resource request on Route %d -- %d requests are now in this queue\n", GetOwnerVtime(MyTimer), MyNum, ResourceId, QueueCard(ResourceQueue[ResourceId]));
	FreeMessage(msg);
	//SendComunicate(MasterId, "\t>Legion %d sent resource request on Route %d\n", MyNum, ResourceId);
	return 0;
}

int FreeResource(int ResourceId) {
	WaitingForResource = 0;
	SendComunicate(MasterId, "%d\t[i] Legion %d will release Route %d\n", GetOwnerVtime(MyTimer), MyNum, ResourceId);
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
	SendComunicate(MasterId, "%d\t\t\t\t[CS]<< %d left CS on route %d\n", GetOwnerVtime(MyTimer),MyNum, RequestedResourceId);
	SendComunicate(MasterId, "%d\t\t[F]<< Legion %d sent free on Route %d -- %d requests are now in this queue\n",GetOwnerVtime(MyTimer), MyNum, ResourceId, QueueCard(ResourceQueue[ResourceId]));
	RequestedResourceId = -1;
	return 0;
}

int ReceiveMessageRoutines() {
	message msg;
	if(NonBlockingReceiveMessage(&msg, REQUEST)) {
		SendComunicate(MasterId, "%d\t\t[R]>> Legion %d received REQUEST from %d on resource %d\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
		SynchronizeVtimers(MyTimer, &(msg.timer));
		IncrementVtimer(MyTimer);

		QueueDeleteIndex(ResourceQueue[msg.resource_id], msg.sender_id);

		if ( (WaitingForResource == 1) && (msg.resource_id == RequestedResourceId) && ( NonConfirmedRequest[msg.sender_id] == 1 ) ) {
			if( (Timestamp > GetOwnerVtime(&(msg.timer))) || ((Timestamp == GetOwnerVtime(&(msg.timer))) && (msg.sender_id < MyNum)) ) {
				QueuePushBeforeOwner(ResourceQueue[msg.resource_id], Node(Pair(msg.sender_id, msg.legion_card)));
				SendComunicate(MasterId, "%d\t\t[L]<< Legion %d let %d go before on route %d, it has %d in queue\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id, QueueCard(ResourceQueue[RequestedResourceId]));
			}
			else {
				QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.sender_id, msg.legion_card)));
				SendComunicate(MasterId, "%d\t\t[W]<< Legion %d has won conflict with %d on route %d, it has %d in queue\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id, QueueCard(ResourceQueue[RequestedResourceId]));
			}
			NonConfirmedRequest[msg.sender_id] = 0;
			--ToConfirm;
			if( (ToConfirm == 1) && ((PredecessorsCard(ResourceQueue[RequestedResourceId]) + MyCard) <= RoutesCapacity[RequestedResourceId]) ) {			
				CriticalSection();
			}
		}
		else {
			SynchronizeVtimers(MyTimer, &(msg.timer));
			IncrementVtimer(MyTimer);
			QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.sender_id, msg.legion_card)));
			message* respond = Message(MyNum, MyCard, *MyTimer, msg.resource_id);
			SendMessage(tids[msg.sender_id], respond, ACK);
			FreeMessage(respond);
			SendComunicate(MasterId, "%d\t\t[A]<< Legion %d respodned %d on route %d with simple ACK\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
		}
	}
	if(NonBlockingReceiveMessage(&msg, FREE)) {
		SynchronizeVtimers(MyTimer, &(msg.timer));
		IncrementVtimer(MyTimer);
		SendComunicate(MasterId, "%d\t\t[F]>> Legion %d received FREE from %d on resource %d\n", GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
		QueueDeleteIndex(ResourceQueue[msg.resource_id], msg.sender_id);
		if(msg.resource_id == RequestedResourceId) {
			SendComunicate(MasterId, "%d\t\t[R]#! Legion %d is recalculating entrance on Route %d\n", GetOwnerVtime(MyTimer), MyNum, msg.resource_id);	
			if( (ToConfirm == 1) && ((PredecessorsCard(ResourceQueue[RequestedResourceId]) + MyCard) <= RoutesCapacity[RequestedResourceId] )) {			
				CriticalSection();
			}	
		}
	}
	if(NonBlockingReceiveMessage(&msg, ACK)) {
		SynchronizeVtimers(MyTimer, &(msg.timer));
		IncrementVtimer(MyTimer);
		SendComunicate(MasterId, "%d\t\t[A]>> Legion %d received ACK from %d on resource %d\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
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
	/*
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&MyNum, 1, 1 );
	pvm_pkint(&MyTid, 1, 1);
	pvm_send(MasterId, SLAVE);
	*/
	/*srand(time(NULL));
	int resource_id = rand()%RouteNum;
	for( int i = 0 ; i < MyNum ; ++i ) {
		resource_id += rand()%RouteNum;
		resource_id %= RouteNum;
	}
	message* core = Message(MyTid, MyCard, *MyTimer, resource_id);
	SendMessage(MasterId, core, COMMUNICATE);
	*/
	/*
	//int k = 10;
	//while(--k) {
		//int enter = rand()%2;
	//	int enter = 1;
		//if((RequestedResourceId == -1) && enter) {
			ResourceRequest(0);//rand()%RouteNum);
			//SendComunicate(MasterId, "%d :: %d wants to enter CS on route %d\n",GetOwnerVtime(MyTimer), MyNum, RequestedResourceId);
		//}
		//ReceiveMessageRoutines();
	//}
	//k = 10;
	while(1) {
		//int enter = rand()%2;
		//int enter = 1;
		//if((RequestedResourceId == -1) && enter) {
		//	FreeResource(0);//rand()%RouteNum);
		//	SendComunicate(MasterId, "%d :: %d leaves CS on route %d - in queue %d left\n",GetOwnerVtime(MyTimer), MyNum, RequestedResourceId, QueueCard(ResourceQueue[0]));
		//}
		ReceiveMessageRoutines();
	}
	SendComunicate(MasterId, "over\n");
	*/

	while(1) {
		int enter = rand()%2;
		//int enter = 1;
		if((RequestedResourceId == -1) && enter) {
			ResourceRequest(rand()%RouteNum);
			SendComunicate(MasterId, "%d :: %d wants to enter CS on route %d\n",GetOwnerVtime(MyTimer), MyNum, RequestedResourceId);
		}
		ReceiveMessageRoutines();
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

