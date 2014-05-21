//<>< P.K.
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "slave.h"
#include "queue.h"

char debug_message[1024];
int RequestedResourceId = -1;
int NonConfirmedRequest[1024];
int ToConfirm = 0;
vtimer* MyTimer;
int MasterId, MyNum, nproc, LegionNum, RouteNum, MyCard;
int MyTid;
queue** ResourceQueue;
int* RoutesCapacity;
int* tids;
int Timestamp = 0;

int CriticalSection() {
	SendComunicate(MasterId, "%d\t\t\t\t[CS]>> %d entered CS on route %d\n",GetOwnerVtime(MyTimer), MyNum, RequestedResourceId);	
	double tstart, cs;
	cs = 0;
	tstart = (double)clock()/CLOCKS_PER_SEC;
	while(cs < 1) {
		cs = (double)clock()/CLOCKS_PER_SEC - tstart;
	}
	int tmp = RequestedResourceId;
	SendComunicate(MasterId, "%d\t\t\t\t[CS]<< %d left CS on route %d\n",GetOwnerVtime(MyTimer), MyNum, tmp);	
	FreeResource(tmp);
	return 0;
}

int ResourceRequest(int ResourceId) {
	RequestedResourceId = ResourceId;
	ToConfirm = nproc - 1;
	for(int i = 0 ; i < nproc ; ++i) {
		NonConfirmedRequest[i] = 1;
	}
	QueuePush(ResourceQueue[ResourceId], Node(Pair(MyNum, MyCard)));
	IncrementVtimer(MyTimer);
	Timestamp = GetOwnerVtime(MyTimer);
	message* msg = Message(MyNum, MyCard, *MyTimer, ResourceId, REQUEST);
	for(int i = 0 ; i < nproc ; ++i) {
		if( i!=MyNum ) {
			SendMessage(tids[i], msg, REQUEST);
		}
	}
	FreeMessage(msg);
	SendComunicate(MasterId, "%d\t[i] %d sent REQUEST on route %d\n",GetOwnerVtime(MyTimer), MyNum, ResourceId);
	while( ToConfirm ) {
		ReceiveMessageRoutines();
	}
	if( (PredecessorsCard(ResourceQueue[ResourceId]) + MyCard) < RoutesCapacity[ResourceId] ) {
		CriticalSection();
	}
	return 0;
}

int FreeResource(int ResourceId) {
	ToConfirm = -1;
	RequestedResourceId = -1;
	IncrementVtimer(MyTimer);
	QueueDeleteIndex(ResourceQueue[ResourceId], MyNum);
	message* msg = Message(MyNum, MyCard, *MyTimer, ResourceId, FREE);
	for(int i = 0 ; i < nproc ; ++i) {
		if( i!=MyNum ) {
			SendMessage(tids[i], msg, REQUEST);
		}
	}
	FreeMessage(msg);
	SendComunicate(MasterId, "%d\t\t[F]<< Legion %d sent free on Route %d -- %d requests are now in this queue\n",GetOwnerVtime(MyTimer), MyNum, ResourceId, QueueCard(ResourceQueue[ResourceId]));
	return 0;
}

int ReceiveMessageRoutines() {
	message msg;
	if(NonBlockingReceiveMessage(&msg, REQUEST)) {		
		SynchronizeVtimers(MyTimer, &(msg.timer));
		IncrementVtimer(MyTimer);
		if(msg.type == REQUEST) {
			SendComunicate(MasterId, "%d\t\t[R]>> Legion %d received REQUEST from %d on resource %d\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
			if(
				(msg.resource_id == RequestedResourceId) &&
				(NonConfirmedRequest[msg.sender_id] == 1)
			) {
				if(
					(Timestamp > GetOwnerVtime(&(msg.timer))) ||
					(
						(Timestamp == GetOwnerVtime(&(msg.timer))) &&
					 	(MyNum > msg.sender_id)
					)
				) {
					QueuePushBeforeOwner(ResourceQueue[msg.resource_id], Node(Pair(msg.sender_id, msg.legion_card)));
					SendComunicate(MasterId, "%d\t\t[L]<< Legion %d let %d go before on route %d, it has %d in queue\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id, QueueCard(ResourceQueue[RequestedResourceId]));

				}
				else {
					QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.sender_id, msg.legion_card)));
					SendComunicate(MasterId, "%d\t\t[W]<< Legion %d has won conflict with %d on route %d, it has %d in queue\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id, QueueCard(ResourceQueue[RequestedResourceId]));

				}
				NonConfirmedRequest[msg.sender_id] = 0;
				--ToConfirm;
			}
			else {
				QueuePush(ResourceQueue[msg.resource_id], Node(Pair(msg.sender_id, msg.legion_card)));
				IncrementVtimer(MyTimer);
				message* response = Message(MyNum, MyCard, *MyTimer, msg.resource_id, ACK);
				SendMessage(tids[msg.sender_id], response, REQUEST);
				SendComunicate(MasterId, "%d\t\t[A]<< Legion %d respodned %d on route %d with simple ACK\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
				FreeMessage(response);
			}

		}

		if(msg.type == FREE) {
			QueueDeleteIndex(ResourceQueue[msg.resource_id], msg.sender_id);
			SendComunicate(MasterId, "%d\t\t[F]>> Legion %d received FREE from %d on resource %d\n", GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
			if ( (msg.resource_id == RequestedResourceId) &&
				  !ToConfirm && 
				  (PredecessorsCard(ResourceQueue[RequestedResourceId]) + MyCard) < RoutesCapacity[RequestedResourceId]
			) {
				SendComunicate(MasterId, "%d\t\t[R]#! Legion %d recalcaled queue and is allowed to enter on Route %d\n", GetOwnerVtime(MyTimer), MyNum, msg.resource_id);	
				CriticalSection();
			}

		}

		if(msg.type == ACK) {
			IncrementVtimer(MyTimer);
			--ToConfirm;
			NonConfirmedRequest[msg.sender_id] = 0;
			SendComunicate(MasterId, "%d\t\t[A]>> Legion %d received ACK from %d on resource %d\n",GetOwnerVtime(MyTimer), MyNum, msg.sender_id, msg.resource_id);
		}
	}
	return 0;
}


int main() {
	srand((int)time(NULL));
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
	while(1) {
		int enter = rand()%2;
		if((RequestedResourceId == -1) && enter) {
			int choice = rand()%RouteNum;
			SendComunicate(MasterId, "%d :: %d wants to enter CS on route %d\n",GetOwnerVtime(MyTimer), MyNum, choice);
			ResourceRequest(choice);
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

