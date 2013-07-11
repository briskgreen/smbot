#ifndef _SMBOT_H
#define _SMBOT_H

#include "dict.h"
#include "query_ip.h"
#include "get_man.h"
#include "get_time.h"
#include "torrent.h"
#include "youku.h"
#include "bt.h"
#include <signal.h>

#define SER "irc.freenode.net"
#define PORT 6667
#define NICK "NICK smbot\r\n"
#define USER "USER smbot sbmot irc.freenode.net :smbot\r\n"
#define JOIN "JOIN #linuxba\r\n"
#define MSG "PRIVMSG "
#define CHANNEL "#linuxba"

int sockfd;
pid_t pid;

void msgto(int sockfd,const char *channel,const char *nick,
		const char *msg);

char *get_nick(char *msg);

#endif
