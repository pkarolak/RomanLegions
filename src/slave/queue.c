//<>< P.K.
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

pair* Pair(int in_id, int in_card) {
	pair* p = calloc(1, sizeof(pair));
	p->id = in_id;
	p->card = in_card;
	return p;
}

int FreePair(pair* p) {
	free(p);
	return 0;
}

node* Node(pair* p) {
	node* n = calloc(1, sizeof(node));
	n->legion = p;
	n->previous = NULL;
	n->next = NULL;
	return n;
}

int FreeNode(node* n) {
	FreePair(n->legion);
	free(n);
	return 0;
}

queue* Queue(int in_owner_id) {
	queue* q = calloc(1, sizeof(queue));
	q->owner_id = in_owner_id;
	q->owner_node = NULL;
	q->first = NULL;
	q->last = NULL;
	return q;
}

int FreeQueue(queue* q) {
	node* following = q->first;
	node* tmp = NULL;
	while(following != NULL) {
		tmp = following->next;
		FreeNode(following);
		following = tmp;
	}
	free(q);
	return 0;
}

int QueuePush(queue* q, node* n) {
	if( n->legion->id == q->owner_id ){
		q->owner_node = n;
	}
	n->previous = q->last;
	if ( q->last != NULL ) {
		q->last->next = n;
	}
	if ( q->first == NULL ) {
		q->first = n;
	}
	q->last = n;
	return 0;
}

int QueuePushBeforeOwner(queue* q, node* n) {
	if( q->owner_node == NULL ) {
		QueuePush(q,n);
	}
	else {
		n->next = q->owner_node;
		q->owner_node->previous->next = n;
		n->previous = q->owner_node->previous;
		q->owner_node->previous = n;
	}
	return 0;
}

int QueueDeleteIndex(queue* q, int legion_id) {
	node* n = q->first;
	while( (n->next != NULL) && (n->legion->id != legion_id) ) {
		n = n->next;
	}
	if ( n->legion->id != legion_id ) {
		return 1;
	}
	if ( n->legion->id == q->owner_id) {
		q->owner_node = NULL;
	}
	if ( (n->previous != NULL) && (n->next != NULL) ) {
		n->previous->next = n->next;
		n->next->previous = n->previous;
	}
	if ( (n->previous != NULL) && (n->next == NULL) ) {
		n->previous->next = NULL;
		q->last = n->previous;
	}
	if ( (n->previous == NULL) && (n->next != NULL) ) {
		n->next->previous = NULL;
		q->first = n->next;
	}
	if ( (n->previous == NULL) && (n->next == NULL) ) {
		q->first = NULL;
		q->last = NULL;
	}
	FreeNode(n);
	return 0;
}


int QueueDeleteNode(queue* q, node* n) {
	if ( n->legion->id == q->owner_id) {
		q->owner_node = NULL;
	}
	if ( (n->previous != NULL) && (n->next != NULL) ) {
		n->previous->next = n->next;
		n->next->previous = n->previous;
	}
	if ( (n->previous != NULL) && (n->next == NULL) ) {
		n->previous->next = NULL;
		q->last = n->previous;
	}
	if ( (n->previous == NULL) && (n->next != NULL) ) {
		n->next->previous = NULL;
		q->first = n->next;
	}
	if ( (n->previous == NULL) && (n->next == NULL) ) {
		q->first = NULL;
		q->last = NULL;
	}
	FreeNode(n);
	return 0;
}

void PrintNode(node* n) {
	printf("#node <Id: %d, Card: %d>\n", n->legion->id, n->legion->card);
}

void PrintQueue(queue* q) {
	node* n = q->first;
	printf("\n");
	while( n != NULL ) {
		PrintNode(n);
		n = n->next;
	}
	printf("\n");
}

int PredecessorsCard(queue* q) {
	node* n = q->first;
	int sum = 0;
	while( (n != NULL) && (n->legion->id != q->owner_id) ) {
		sum += n->legion->card;
		n = n->next;
	}
	return sum;
}