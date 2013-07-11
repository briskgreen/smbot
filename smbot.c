#include "smbot.h"

void msgto(int sockfd,const char *channel,const char *nick,const char *msg)
{
	int len;
	char *buf;

	if(nick == NULL)
		len=strlen(MSG)+strlen(channel)+strlen(msg)+3;
	else
		len=strlen(MSG)+strlen(channel)+strlen(nick)+strlen(msg)+5;
	buf=malloc(len);
	bzero(buf,len);

	strncpy(buf,MSG,strlen(MSG));
	strncat(buf,channel,strlen(channel));
	strncat(buf," :",2);
	if(nick != NULL)
	{
		strncat(buf,nick,strlen(nick));
		strncat(buf,": ",2);
	}
	strncat(buf,msg,strlen(msg));

	send(sockfd,buf,strlen(buf),0);

	free(buf);
	buf=NULL;
}

char *get_nick(char *msg)
{
	char *buf=malloc(sizeof(int)*30);
	int i,j;

	for(i=1,j=0;msg[i];++i,++j)
		if(msg[i] == '!')
			break;
		else
			buf[j]=msg[i];

	buf[j]='\0';

	return buf;
}

