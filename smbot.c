#include "smbot.h"

void msgto(int sockfd,const char *channel,const char *nick,const char *msg)
{
	int len;
	char *buf=NULL;
	fd_set writes;

	if(channel == NULL)
		return;
	if(nick == NULL)
		len=strlen(MSG)+strlen(channel)+strlen(msg)+3;
	else
		len=strlen(MSG)+strlen(channel)+strlen(nick)+strlen(msg)+5;
	buf=malloc(len);
	buf[len-1]='\0';

	if(nick != NULL)
		sprintf(buf,"%s%s :%s: %s",MSG,channel,nick,msg);
	else
		sprintf(buf,"%s%s :%s",MSG,channel,msg);

	FD_ZERO(&writes);
	FD_SET(sockfd,&writes);
	len=select(sockfd+1,NULL,&writes,NULL,NULL);
	if(len == -1)
	{
		printf("Error send\n");
		return;
	}

	send(sockfd,buf,strlen(buf),0);

	safe_free(&buf);
}

char *get_nick(char *msg)
{
	char *buf=NULL;
	int len;

	for(len=1;msg[len] != '!';++len);
	buf=malloc(len);
	strncpy(buf,msg+1,len-1);
	buf[len-1]='\0';

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
	{
		regfree(&preg);
		return NULL;
	}
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

	for(index+=2,len=1;buf[index];++len,++index);
	res=malloc(len);
	strncpy(res,buf+pmatch[0].rm_eo+1,len-1);
	res[len-1]='\0';

	return res;
}

void pong_ser(int sockfd,char *msg)
{
	msg[1]='O';
	send(sockfd,msg,strlen(msg),0);
}

char *get_channel(char *msg)
{
	char *buf;
	regex_t reg;
	regmatch_t pmatch[1];
	int len;

	if(regcomp(&reg,"#.[^ ]*",0) != 0)
	{
		regfree(&reg);

		return NULL;
	}
	if(regexec(&reg,msg,1,pmatch,0) != 0)
	{
		regfree(&reg);

		return NULL;
	}
	regfree(&reg);

	len=pmatch[0].rm_eo-pmatch[0].rm_so+1;
	buf=malloc(len);
	strncpy(buf,msg+pmatch[0].rm_so,len-1);
	buf[len-1]='\0';

	return buf;
}
