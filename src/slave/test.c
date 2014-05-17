#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "queue.h"

int main (int argc, char **argv)
{
  int owner = 13;
  queue* q = Queue(owner);
  pair* p;
  node* n;
  for(int i = 0 ; i < 14; ++i) {
    p = Pair(i, rand()%100);
    n = Node(p);
    QueuePush(q, n);
  }
  QueuePushBeforeOwner(q, Node(Pair(500, 1000)));
  PrintQueue(q);
  int sum = PredecessorsCard(q);
  printf("\n\nSUM: %d\n", sum);
  FreeQueue(q);
  return 0;
}