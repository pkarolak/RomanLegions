#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct pair {
	int id;
	int card; 
} pair;

typedef struct node {
	struct pair* legion;
	struct node* previous;
	struct node* next;
} node;

typedef struct queue {
	int owner_id;
	struct node* owner_node;
	struct node* first;
	struct node* last;
} queue;

pair* Pair(int id, int card);
int FreePair(pair* p);
node* Node(pair* p);
int FreeNode(node* n);
queue* Queue(int in_owner_id);
int FreeQueue(queue* q);
int QueuePush(queue* q, node* n);
int QueuePushBeforeOwner(queue* q, node* n);
int QueueDeleteIndex(queue* q, int legion_id);
int QueueDeleteNode(queue* q, node* n);
void PrintNode(node* n);
void PrintQueue(queue* q);
int PredecessorsCard(queue* q);

#endif