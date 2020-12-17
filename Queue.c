#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int SUCCESS = 0;
int FAILURE = -1;

typedef struct Element
{
	int index;
	struct Element* next;
} Element;

typedef struct Queue
{
	struct Element* front;
	struct Element* rear;
} Queue;

Queue* InitializeQueue() 
{
	Queue* new_q = (Queue*)malloc(sizeof(Queue));
	if (new_q == NULL) 
	{
		printf("Memory Error - Couldn't initialize queue. ABORT.");
		return NULL;
	}
	new_q->front = NULL;
	new_q->rear = NULL;
	return new_q;
}

bool Empty(Queue* q)
{
	if (q->front == NULL) 
	{
		return true;
	}
	else
	{
		return false;
	}
}

Element* Top(Queue* q) 
{
	if (Empty(q))
	{
		printf("Queue is empty.");
		return NULL;
	}
	printf("returns element with index: %d\n", q->front->index);
	return q->front;
}

int Pop(Queue* q)
{
	if (Empty(q))
	{
		printf("Can not Pop. Queue is empty.");
		return FAILURE;
	}

	q->front = q->front->next;
	
	if (NULL == q->front)
	{
		q->rear = NULL;
	}
	return SUCCESS;
}

int DestoryQueue(Queue* q)
{
	while (Empty(q) != true)
	{
		Pop(q);
		printf("Popped!\n");
	}
	free(q);
	q = NULL;
	printf("Queue has been destroyed successfully!\n");
	return SUCCESS;
}

void Push(Queue* q, Element* p_element)
{	
	if (Empty(q)) 
	{
		q->front = p_element;
		q->front->next = NULL;
		q->rear = q->front;
		q->rear->next = NULL;
		return;
	}
	else
	{
		Element* temp = q->front;
		while (temp != NULL)
		{
			if (p_element == temp)
			{
				printf("This element is already in the queue, returning.\n");
				return;
			}
			temp = temp->next;
		}
		q->rear->next = p_element;
		q->rear = p_element;
		q->rear->next = NULL;
		return;
	}
}

// We have used a code from https://www.geeksforgeeks.org/queue-linked-list-implementation/
int main()
{
	Queue* q = InitializeQueue();
	if (NULL == q) 
	{
		printf("Memory Error - Couldn't initialize queue. ABORT.");
		return FAILURE;
	}
	if (Empty(q))
	{
		printf("Queue is empty!\n");
	}
	else
		printf("Queue is not empty!\n");
	Element elem = { .index = 1, .next = NULL };
	Element elem2 = { .index = 2, .next = NULL };
	Element elem3 = { .index = 3, .next = NULL };
	Element elem4 = { .index = 4, .next = NULL };
	Element elem5 = { .index = 5, .next = NULL };
	
	Push(q, &elem);
	Push(q, &elem2);
	Top(q);
	Push(q, &elem3);
	Pop(q);
	Push(q, &elem4);
	Push(q, &elem4);
	Pop(q);
	Pop(q);
	Top(q);

	DestoryQueue(q);

	return SUCCESS;
}