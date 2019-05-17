#include <stdio.h>
#include <stdlib.h>
#include "../types.h"
struct Node {
	tlv_request_t data;
	struct Node *next;
};
 
struct Queue {
	struct Node *front;
	struct Node *last;
	unsigned int size;
};
 
void init(struct Queue *q);
tlv_request_t front(struct Queue *q);
void pop(struct Queue *q);
void push(struct Queue *q, tlv_request_t data);