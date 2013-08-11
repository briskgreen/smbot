#ifndef _SMBOT_H
#define _SMBOT_H

#include "TaskFactory/taskfactory.h"
#include "mysock/mysock.h"
#include <signal.h>

#define QUIT_MSG "如果一切都变成了或许，谁还会记得曾经\n"

int sockfd;
SSL *ssl;

typedef struct
{
	char *nick;
	char *channel;
	char *arg;
	bool is_use_ssl;
}SMBOT_DATA;

void msgto(int sockfd,const char *channel,const char *nick,
		const char *msg);

void ssl_msgto(SSL *ssl,const char *channel,const char *nick,
		const char *msg);

char *get_nick(char *msg);

char *get_arg(char *buf,char *prg,char *des);

void pong_server(char *msg,bool is_use_ssl);

char *get_channel(char *msg);

void smbot_help(bool is_use_ssl);

void parse_and_perform(TASK_FACTORY *task,char *msg,char *reg,
		char *des,task_callback func,bool is_use_ssl,
		unsigned int priority);

char *match_string(char *reg,char *data);

void smbot_destory(SMBOT_DATA *data);

void null_no_free(char *p);

#endif
