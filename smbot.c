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

	if(nick != NULL)
		sprintf(buf,"%s%s :%s: %s",MSG,channel,nick,msg);
	else
		sprintf(buf,"%s%s :%s",MSG,channel,msg);

	send(sockfd,buf,len-1,0);

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

