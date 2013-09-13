#ifndef _IFSMBOT_CONFIG_H
#define _IFSMBOT_CONFIG_H

#include "command.h"

#define SMBOT_CONF_PATH ".config"

typedef struct
{
	FILE *fp;
	char *nick;
	char *user_name;
	char *passwd;
	char *really_name;
	char *use_ssl;
	char *server;
	unsigned int port;
	CHANNEL *channel;
}SMBOT_CONF;

SMBOT_CONF *smbot_conf_init(void);

bool smbot_conf_open(SMBOT_CONF *conf);

void smbot_conf_read(SMBOT_CONF *conf);

void smbot_conf_close(SMBOT_CONF *conf);

#endif
