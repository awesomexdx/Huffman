#pragma once
#include "stdio.h"
#include "stdlib.h"

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

//#define DEBUG_OUTPUT

//#define BIT_OUTPUT

#define BUFF_SIZE   128

typedef struct st_tree_node SoR;
struct st_tree_node {
	unsigned char data; // ASCII CODE
	SoR * left;
	SoR * right;
};


typedef struct {
    FILE * file;
    unsigned int bit_pos; //позиция бита в байте
    unsigned int byte_pos; //

    unsigned char buff[BUFF_SIZE];

} BitIOStruct;


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