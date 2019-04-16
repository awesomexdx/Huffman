#pragma once
#include "stdio.h"
#include "stdlib.h"

#define BUFF_SIZE   1024

typedef struct st_tree_node SoR;
struct st_tree_node {
	unsigned char data; // ASKII CODE
	SoR * left;
	SoR * right;
};

typedef struct {
	char * code;
	size_t length;
} Code;

typedef struct st_queueitem QueueItem;
struct st_queueitem {
	unsigned int priority; //Приоритет элемента
	SoR *value; // Данные
};

typedef struct st_queue Queue;
struct st_queue {

	unsigned int maxsize; // Максимальный размер массива
	unsigned int nitems; // Количество элементов в куче

	QueueItem *items;
};

Queue *heap_create(unsigned int maxsize);

void queueFree(Queue *h);
int queueInsert(Queue *h, unsigned int priority, SoR *value);
QueueItem getMin(Queue *h);