#include "taskfactory.h"

typedef struct
{
	TASK_FACTORY *task;
	task_callback task_func;
	void *data;
}DATA;

pthread_mutex_t task_factory_mutex;
pthread_cond_t task_factory_cond;

void task_factory(TASK_FACTORY *task);
void task_add(DATA *data);

char *task_error(int error_code)
{
	switch(error_code)
	{
		case TASK_FULL:
			return "The Task Full!\n";
		case TASK_EMPTY:
			return "The Task Empty!\n";
		case TASK_THREAD:
			return "Create Thread Error!\n";
		default:
			return "No Error\n";
	}
}

TASK_QUEUE *task_queue_init(unsigned int max)
{
	TASK_QUEUE *head;

	head=malloc(sizeof(TASK_QUEUE));
	head->max=max;
	head->len=0;
	head->next=NULL;

	return head;
}

int task_queue_add(TASK_QUEUE *head,task_callback task_func,
		void *data,unsigned int priority)
{
	TASK_QUEUE_NODE *temp;
	TASK_QUEUE_NODE *node;

	if(head->len >= head->max)
		return TASK_FULL;

	temp=head;
	while(temp->next != NULL)
	{
		if(temp->next->priority <= priority)
			temp=temp->next;
		else
			break;
	}

	node=malloc(sizeof(TASK_QUEUE_NODE));
	node->data=data;
	node->priority=priority;
	node->func=task_func;

	if(temp->next == NULL)
		node->next=NULL;
	else
		node->next=temp->next;
	temp->next=node;
	++head->len;
	return 0;
}

int task_queue_out(TASK_QUEUE *head,TASK_QUEUE_NODE *data)
{
	TASK_QUEUE_NODE *temp;

	if(head->len <= 0)
		return TASK_EMPTY;

	temp=head->next;
	memcpy(data,temp,sizeof(TASK_QUEUE_NODE));
	head->next=temp->next;
	//free(temp);
	--head->len;
	return 0;
}

bool task_queue_is_empty(TASK_QUEUE *head)
{
	if(head->len <= 0)
		return TRUE;
	else
		return FALSE;
}

bool task_queue_is_full(TASK_QUEUE *head)
{
	if(head->len >= head->max)
		return TRUE;
	else
		return FALSE;
}

unsigned int task_queue_length(TASK_QUEUE *head)
{
	return head->len;
}

void task_queue_set_max_task(TASK_QUEUE *head,unsigned int max)
{
	head->max=max;
}

unsigned int task_queue_get_max_task(TASK_QUEUE *head)
{
	return head->max;
}

void task_queue_destroy(TASK_QUEUE *head)
{
	TASK_QUEUE_NODE *temp;
	TASK_QUEUE_NODE *node;

	temp=head->next;

	while(temp != NULL)
	{
		node=temp;
		free(node);
		temp=temp->next;
	}

	free(head);
}

TASK_FACTORY *task_factory_init(unsigned int task_max,
		unsigned int queue_max)
{
	TASK_FACTORY *task;

	task=malloc(sizeof(TASK_FACTORY));
	task->max=task_max;
	task->len=0;
	task->head=task_queue_init(queue_max);
	pthread_mutex_init(&task_factory_mutex,NULL);
	pthread_cond_init(&task_factory_cond,NULL);
	if(pthread_create(&task->thread,NULL,(void *)task_factory,task) == -1) 
	{
		free(task);
		return NULL;
	}

	return task;
}

int task_factory_add(TASK_FACTORY *task,
		task_callback task_func,void *data,
		unsigned int priority)
{
	pthread_t thread;
	DATA *d;

	if(task_factory_is_full(task))
	{
		task_queue_add(task->head,task_func,data,priority);
		return TASK_ADD_QUEUE;
	}

	d=malloc(sizeof(DATA));
	d->task=task;
	d->task_func=task_func;
	d->data=data;

	if(pthread_create(&thread,NULL,(void *)task_add,d) == -1)
		return TASK_THREAD;

	++task->len;
	return TASK_OK;
}

void task_factory_finished(TASK_FACTORY *task)
{
	while(pthread_cond_signal(&task_factory_cond) != 0);
	if(task->len != 0)
		--task->len;
}

void task_factory_set_max_task(TASK_FACTORY *task,unsigned int max)
{
	task->max=max;
}

unsigned int task_factory_get_max_task(TASK_FACTORY *task)
{
	return task->max;
}

unsigned int task_factory_get_length(TASK_FACTORY *task)
{
	return task->len;
}

bool task_factory_is_full(TASK_FACTORY *task)
{
	if(task->len >= task->max)
		return TRUE;
	else
		return FALSE;
}

void task_factory_destroy(TASK_FACTORY *task)
{

	task_queue_destroy(task->head);
	task->max=0;
	task->len=0;
	pthread_cancel(task->thread);
	pthread_mutex_destroy(&task_factory_mutex);
	pthread_cond_destroy(&task_factory_cond);
	free(task);
}

void task_factory(TASK_FACTORY *task)
{
	TASK_QUEUE_NODE data;

	while(1)
	{
		while(pthread_cond_wait(&task_factory_cond,&task_factory_mutex) != 0);

		if(task_queue_out(task->head,&data) == TASK_EMPTY)
			continue;

		task_factory_add(task,data.func,data.data,data.priority);
	}
}

void task_add(DATA *data)
{
	pthread_mutex_lock(&task_factory_mutex);
	data->task_func(data->data);
	pthread_mutex_unlock(&task_factory_mutex);
	task_factory_finished(data->task);
	free(data);
	pthread_exit(NULL);
}
