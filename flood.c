#include "flood.h"

unsigned int hash_fun(char *key);
void fd_insert_to_overflow(LIST *list,FD *data);
FD *fd_get_in_overflow(LIST *list,char *ip);
int flood(LIST *list,FD *DATA);
int index_in_list(LIST *list,char *ip);

int fd_insert(LIST *list,FD *data)
{
	FILE *fp;
	int h;
	int len;
	char path[256];

	if(fd_get(list,data->ip))
		return -1;

	h=hash_fun(data->ip)%HASH_LEN;
	snprintf(path,256,"%s%u",FD_PATH,h);
	if(access(path,R_OK) != 0)
	{
		if((fp=fopen(path,"wb")) == NULL)
			return -2;
		len=1;
		fwrite(&len,sizeof(int),1,fp);
		fwrite(data,sizeof(FD),1,fp);
		fclose(fp);
	}
	else
	{
		if((fp=fopen(path,"r+")) == NULL)
			return -2;
		fread(&len,sizeof(int),1,fp);
		if(len >= DATA_MAX)
		{
			fclose(fp);
			fd_insert_to_overflow(list,data);
			return 0;
		}

		++len;
		rewind(fp);
		fwrite(&len,sizeof(int),1,fp);
		fseek(fp,0L,SEEK_END);
		fwrite(data,sizeof(FD),1,fp);
		fclose(fp);
	}

	free(data);
}

FD *fd_get(LIST *list,char *ip)
{
	int h;
	FILE *fp;
	FD *res;
	FD *node;
	int len;
	int i;
	char path[256];

	if((res=list_get(list,ip)))
		return res;

	h=hash_fun(ip)%HASH_LEN;
	snprintf(path,256,"%s%u",FD_PATH,h);
	if(access(path,R_OK) != 0)
		return NULL;

	if((fp=fopen(path,"rb")) == NULL)
		return NULL;
	fread(&len,sizeof(int),1,fp);
	list_cleanup(list);
	list->hash=h;

	for(i=0;i < len;++i)
	{
		node=malloc(sizeof(FD));
		fread(node,sizeof(FD),1,fp);
		list_append(list,node);
	}

	fclose(fp);
	if(res=list_get(list,ip))
		return res;
	else if(len >= DATA_MAX)
		return fd_get_in_overflow(list,ip);
	else
		return NULL;
}

LIST *list_init(void)
{
	LIST *list;

	list=malloc(sizeof(LIST));
	list->hash=-2;
	list->data=NULL;
	list->next=NULL;

	return list;
}

void list_append(LIST *list,FD *data)
{
	LIST *node;

	if(list->data == NULL)
	{
		list->data=data;
		return;
	}

	while(list->next)
		list=list->next;

	node=malloc(sizeof(LIST));
	node->data=data;
	node->next=NULL;
	list->next=node;
}

FD *list_get(LIST *list,char *ip)
{
	if(list->data == NULL)
		return NULL;

	while(list)
	{
		if(strcmp(list->data->ip,ip) == 0)
			return list->data;

		list=list->next;
	}

	return NULL;
}

void list_cleanup(LIST *list)
{
	LIST *head=list;
	LIST *temp;

	list=list->next;
	while(list)
	{
		temp=list;
		list=list->next;
		free(temp->data);
		free(temp);
	}

	free(head->data);
	head->hash=-2;
	head->data=NULL;
	head->next=NULL;
}

int flood_test(LIST *list,char *ip,char *nick)
{
	FD *node;

	if(node=fd_get(list,ip))
		return flood(list,node);

	node=malloc(sizeof(FD));
	snprintf(node->ip,100,"%s",ip);
	snprintf(node->nick,17,"%s",nick);
	node->time=time(NULL);
	node->count=0;
	node->flood=0;
	fd_insert(list,node);

	return 0;
}

void fd_insert_to_overflow(LIST *list,FD *data)
{
	FILE *fp;
	int len;
	int i;

	if(access(FD_OVERFLOW,R_OK) != 0)
	{
		if((fp=fopen(FD_OVERFLOW,"wb")) == NULL)
			return;
		len=1;
		fwrite(&len,sizeof(int),1,fp);
		fclose(fp);
	}
	else
	{
		if((fp=fopen(FD_OVERFLOW,"r+")) == NULL)
			return;
		fread(&len,sizeof(int),1,fp);
		++len;
		rewind(fp);
		fwrite(&len,sizeof(int),1,fp);
		fseek(fp,0L,SEEK_END);
		fwrite(data,sizeof(FD),1,fp);
		fclose(fp);
	}

	free(data);
}

FD *fd_get_in_overflow(LIST *list,char *ip)
{
	FILE *fp;
	FD *res;
	FD *node;
	int len;
	int i;

	if(res=list_get(list,ip))
		return res;

	if(access(FD_OVERFLOW,R_OK) != 0)
		return NULL;

	if((fp=fopen(FD_OVERFLOW,"rb")) == NULL)
		return NULL;
	fread(&len,sizeof(int),1,fp);
	list_cleanup(list);
	list->hash=-1;

	for(i=0;i < len;++i)
	{
		node=malloc(sizeof(FD));
		fread(node,sizeof(FD),1,fp);
		list_append(list,node);
	}
	
	fclose(fp);

	res=list_get(list,ip);

	return res;
}

int flood(LIST *list,FD *data)
{
	time_t now;
	FILE *fp;
	int index;
	char path[256];

	now=time(NULL);
	if(data->flood > 0)
	{
		if(now-data->time > 60*data->flood)
		{
			data->flood=0;
			data->count=0;
			data->time=now;
		}
	}

	++data->count;
	if(data->count%5 == 0)
	{
		if(now-data->time < 7)
			++data->flood;

		if(data->count/5 > 1)
			++data->flood;
	}

	if(data->count % 65535 == 0)
		data->count=6;

	if(list->hash == -1)
	{
		index=index_in_list(list,data->ip);
		if(index == -1)
			return -1;
		if((fp=fopen(FD_OVERFLOW,"r+")) == NULL)
			return -1;
	}
	else
	{
		snprintf(path,256,"%s%u",FD_PATH,list->hash);
		index=index_in_list(list,data->ip);
		if(index == -1)
			return -1;
		if((fp=fopen(path,"r+")) == NULL)
			return -1;
	}

	fseek(fp,sizeof(int)+sizeof(FD)*index,SEEK_SET);
	fwrite(data,sizeof(FD),1,fp);
	fclose(fp);

	return data->flood;
}

int index_in_list(LIST *list,char *ip)
{
	int i=0;

	while(list)
	{
		if(strcmp(list->data->ip,ip) == 0)
			return i;

		list=list->next;
		++i;
	}

	return -1;
}

unsigned int hash_fun(char *key)
{
	unsigned int seed=131;
	unsigned int hash=0;

	while(*key)
	{
		hash=hash*seed+*key;
		++key;
	}

	return hash&0x7FFFFFFF;
}
