#ifndef _SMBOT_H
#define _SMBOT_H

#include "TaskFactory/taskfactory.h"
#include "mysock/mysock.h"
#include <signal.h>

#define QUIT_MSG "如果一切都变成了或许，谁还会记得曾经\n"

int sockfd;
SSL *ssl;

void msgto(int sockfd,const char *channel,const char *nick,
		const char *msg);

char *get_nick(char *msg);

char *get_arg(char *buf,char *prg,char *des);

void pong_server(char *msg,bool is_use_ssl);

char *get_channel(char *msg);

void smbot_help(bool is_use_ssl);

#endif
