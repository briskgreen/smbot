#include "smbot.h"

void msgto(int sockfd,const char *channel,const char *nick,const char *msg)
{
	int len;
	char *buf;
	fd_set writes;

	if(nick == NULL)
		len=strlen("PRIVMSG ")+strlen(channel)+strlen(msg)+3;
	else
		len=strlen("PRIVMSG ")+strlen(channel)+strlen(nick)+strlen(msg)+5;
	buf=malloc(len);
	buf[len]='\0';

	if(nick != NULL)
		sprintf(buf,"PRIVMSG %s :%s: %s",channel,nick,msg);
	else
		sprintf(buf,"PRIVMSG %s :%s",channel,msg);

	FD_ZERO(&writes);
	FD_SET(sockfd,&writes);
	len=select(sockfd+1,NULL,&writes,NULL,NULL);
	if(len == -1)
	{
		printf("Error send\n");
		return;
	}

	send(sockfd,buf,strlen(buf),0);

	free(buf);
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

char *get_arg(char *buf,char *prg,char *des)
{
	regex_t preg;
	regmatch_t pmatch[1];
	int nmatch=1;
	char *res;
	int index;
	int len;
	char temp;

	if(regcomp(&preg,prg,0) != 0)
		return NULL;
	if(regexec(&preg,buf,nmatch,pmatch,0) !=0)
	{
		regfree(&preg);
		return NULL;
	}
	regfree(&preg);

	index=pmatch[0].rm_eo;
	temp=buf[index];
	if(temp != ' ' && temp == '\r')
		return des;
	else if(temp != ' ' && temp != '\r')
		return NULL;

	for(index+=2,len=0;buf[index];++len,++index);
	res=malloc(len+1);
	bzero(res,len+1);
	strncpy(res,buf+pmatch[0].rm_eo+1,len);

	return res;
}

