#ifndef _TASKFACTORY_H
#define _TASKFACTORY_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define TASK_FULL -1
#define TASK_EMPTY -2
#define TASK_ADD_QUEUE -3
#define TASK_THREAD -4
#define TASK_OK 0

typedef void (*task_callback)(void *data);
typedef unsigned char bool;

typedef struct node
{
	void *data;
	unsigned int priority;
	struct node *next;
	task_callback func;
}TASK_QUEUE_NODE;

typedef struct 
{
	unsigned int max;
	unsigned int len;
	struct node *next;
}TASK_QUEUE;

typedef struct 
{
	unsigned int len;
	unsigned int max;
	TASK_QUEUE *head;
	pthread_t thread;
}TASK_FACTORY;

char *task_error(int error_code);
TASK_QUEUE *task_queue_init(unsigned int max);
int task_queue_add(TASK_QUEUE *head,task_callback task_func,
		void *data,unsigned int priority);
int task_queue_out(TASK_QUEUE *head,TASK_QUEUE_NODE *data);
bool task_queue_is_empty(TASK_QUEUE *head);
bool task_queue_is_full(TASK_QUEUE *head);
unsigned int task_queue_length(TASK_QUEUE *head);
void task_queue_set_max_task(TASK_QUEUE *head,unsigned int max);
unsigned int task_queue_get_max_task(TASK_QUEUE *head);
void task_queue_destroy(TASK_QUEUE *head);

TASK_FACTORY *task_factory_init(unsigned int task_max,unsigned int queue_max);
int task_factory_add(TASK_FACTORY *task,task_callback task_func,
		void *data,unsigned int priority); 
void task_factory_finished(TASK_FACTORY *task);
void task_factory_set_max_task(TASK_FACTORY *task,unsigned int max);
unsigned int task_factory_get_max_task(TASK_FACTORY *task);
unsigned int task_factory_get_length(TASK_FACTORY *task);
bool task_factory_is_full(TASK_FACTORY *task);
void task_factory_destroy(TASK_FACTORY *task);

#endif
