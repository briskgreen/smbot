#ifndef _SMBOT_H
#define _SMBOT_H

#include "dict.h"
#include "query_ip.h"
#include "get_man.h"
#include "get_time.h"
#include "torrent.h"
#include "youku.h"
#include "bt.h"
#include "yb.h"
#include "weather.h"
#include "stack.h"
#include "id.h"
#include "checkid.h"
#include "url.h"
#include "joke.h"
#include <signal.h>
#include <regex.h>

#define SER "irc.freenode.org"
#define PORT 6667
#define NICK "NICK smbot\n"
#define USER "USER smbot sbmot irc.freenode.net :smbot\n"
#define JOIN_DEBIAN_CN "JOIN #debian_cn\n"
#define JOIN_LINUXBA "JOIN #linuxba\n"
#define JOIN_LINUXBAR "JOIN #linuxbar\n"
#define JOIN_LINUKSO "JOIN #linukso\n"
#define MSG "PRIVMSG "

int sockfd;
pid_t pid;

void msgto(int sockfd,const char *channel,const char *nick,
		const char *msg);

char *get_nick(char *msg);

char *get_channel(char *msg);

char *get_arg(char *buf,char *prg,char *des);

void pong_ser(int sockfd,char *msg);

#endif
