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
	printf("Queue created\n");
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
	node* following = q->first;
	while( (following->legion->id != legion_id) && (following->next != NULL) ) {
		following = following->next;
	}
	if ( following == q->last ) {
		following->previous->next = NULL;
		q->last = following->previous;
	}
	else if ( following == q->first ){
		following->next->previous = NULL;
		q->first = following->next;
	}
	else {
		following->next->previous = following->previous;
		following->previous->next = following->next;
	}
	if ( following->legion->id == q->owner_id) {
		q->owner_node = NULL;
	}
	FreeNode(following);
	return 0;
}


int QueueDeleteNode(queue* q, node* n) {
	if ( n == q->last ) {
		n->previous->next = NULL;
		q->last = n->previous;
	}
	else if ( n == q->first ){
		n->next->previous = NULL;
		q->first = n->next;
	}
	else {
		n->next->previous = n->previous;
		n->previous->next = n->next;
	}
	FreeNode(n);
	return 0;
}

void PrintNode(node* n) {
	printf("#node <Id: %d, Card: %d>\n", n->legion->id, n->legion->card);
}

void PrintQueue(queue* q) {
	node* following = q->first;
	while( following != NULL ) {
		PrintNode(following);
		following = following->next;
	}
}