#include "config.h"

void conf_get_channel(CHANNEL *channel,const char *data);

SMBOT_CONF *smbot_conf_init(void)
{
	SMBOT_CONF *conf;

	conf=malloc(sizeof(SMBOT_CONF));
	if(conf == NULL)
		return NULL;

	conf->nick=NULL;
	conf->user_name=NULL;
	conf->really_name=NULL;
	conf->use_ssl=NULL;
	conf->server=NULL;

	return conf;
}

bool smbot_conf_open(SMBOT_CONF *conf)
{
	if((conf->fp=fopen(SMBOT_CONF_PATH,"rb")) == NULL)
		return FALSE;
	if((conf->channel=http_head_init()) == NULL)
		return FALSE;

	return TRUE;
}

void smbot_conf_read(SMBOT_CONF *conf)
{
	int len;
	char *data=NULL;
	char *temp;

	fseek(conf->fp,0L,SEEK_SET);

	getline(&data,&len,conf->fp);
	conf->nick=strnstr(data+5,-1);
	getline(&data,&len,conf->fp);
	conf->user_name=strnstr(data+10,-1);
	getline(&data,&len,conf->fp);
	conf->really_name=strnstr(data+12,-1);
	getline(&data,&len,conf->fp);
	conf->use_ssl=strnstr(data+8,-1);
	getline(&data,&len,conf->fp);
	conf->server=strnstr(data+7,-1);
	getline(&data,&len,conf->fp);
	temp=strnstr(data+5,-1);
	conf->port=atoi(temp);
	free(temp);
	getline(&data,&len,conf->fp);
	conf_get_channel(conf->channel,data+8);
}

void smbot_conf_close(SMBOT_CONF *conf)
{
	fclose(conf->fp);

	free(conf->nick);
	free(conf->user_name);
	free(conf->really_name);
	free(conf->server);
	free(conf->use_ssl);

	http_head_destroy(conf->channel);
	free(conf);
}

void conf_get_channel(CHANNEL *channel,const char *data)
{
	char temp[512]={0};
	int i;
	int n=0;
	char *res;

	for(i=0;data[i];++i)
	{
		temp[n]=data[i];

		if(data[i] == ' ' || data[i] == '\n')
		{
			res=strnstr(temp,n);
			http_head_add(channel,res);
			n=0;
			continue;
		}

		++n;
	}
}
