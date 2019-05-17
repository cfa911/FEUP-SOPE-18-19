#include "queue.h"
void init(struct Queue *q) {
	q->front = NULL;
	q->last = NULL;
	q->size = 0;
}
 
tlv_request_t front(struct Queue *q) {
	return q->front->data;
}
 
void pop(struct Queue *q) {
	q->size--;
 
	struct Node *tmp = q->front;
	q->front = q->front->next;
	free(tmp);
}
 
void push(struct Queue *q, tlv_request_t data) {
	q->size++;
 
	if (q->front == NULL) {
		q->front = (struct Node *) malloc(sizeof(struct Node));
		q->front->data = data;
		q->front->next = NULL;
		q->last = q->front;
	} else {
		q->last->next = (struct Node *) malloc(sizeof(struct Node));
		q->last->next->data = data;
		q->last->next->next = NULL;
		q->last = q->last->next;
	}
}