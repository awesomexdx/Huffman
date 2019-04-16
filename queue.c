#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "sort.h"


Queue *heap_create(unsigned int maxsize)
{
	Queue *h;

	h = malloc(sizeof(*h));
	if (h != NULL) {

		h->maxsize = maxsize;
		h->nitems = 0;
		h->items = malloc(sizeof(QueueItem) * (maxsize + 1));

		if (h->items == NULL) {

			free(h);
			return NULL;
		}
	}
	return h;
}


QueueItem getMin(Queue *h)
{
    if (h->nitems <= 0)
        return h->items[0];

    return h->items[--h->nitems];
}


void queueFree(Queue *h)
{
    if (h == NULL)
        return;

	free(h->items);
	free(h);
}


int queueInsert(Queue *h, unsigned int priority, SoR *value)
{
	if (h->nitems >= h->maxsize) {
		fprintf(stderr, "queue: Heap overflow.\n");
		return -1;
	}

	h->items[h->nitems].priority = priority;
	h->items[h->nitems].value = value;

	quickSort(h->items, 0, h->nitems++);

	return 0;
}