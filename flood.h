#ifndef _FLOOD_H
#define _FLOOD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

//#define FD_PATH(hash) "flood_data/" HASH_TO_STR(hash)
#define FD_PATH "flood_data/"
#define FD_OVERFLOW "flood_data/overflow"

#define HASH_LEN 5000
#define DATA_MAX 50

typedef struct
{
	char ip[100];
	char nick[17];
	time_t time;
	int count;
	int flood;
}__attribute__((packed))FD;

typedef struct list_node
{
	int hash;
	FD *data;

	struct list_node *next;
}LIST;

int fd_insert(LIST *list,FD *data);

FD *fd_get(LIST *list,char *ip);

LIST *list_init(void);

void list_append(LIST *list,FD *data);

FD *list_get(LIST *list,char *ip);

void list_cleanup(LIST *list);

int flood_test(LIST *list,char *ip,char *nick);

#endif
