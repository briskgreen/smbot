#include "smbot.h"

void msgto(int sockfd,const char *channel,const char *nick,
		const char *msg)
{
	int len;
	int n=0;
	char *buf=NULL;
	char *temp;

	len=strlen(msg);

	do
	{
		buf=strnstr(msg+n,400);
		temp=string_add("PRIVMSG %s :%s: %s\n",
				channel,nick,buf+n);

		n=send(sockfd,temp,strlen(temp),0);
		len-=n;
		free(buf);
		free(temp);
	}while(len > 400);
}

void ssl_msgto(SSL *ssl,const char *channel,const char *nick,
		const char *msg)
{
	int len;
	int n=0;
	char *buf=NULL;
	char *temp;

	len=strlen(msg);

	do
	{
		buf=strnstr(msg+n,400);
		temp=string_add("PRIVMSG %s :%s: %s\n",
				channel,nick,buf+n);
		n=SSL_write(ssl,temp,strlen(temp));
		len-=n;
		free(buf);
		free(temp);
	}while(len > 400);
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
	char *res;
	int index;
	int len;
	char temp;

	if(regcomp(&preg,prg,0) != 0)
	{
		regfree(&preg);
		return NULL;
	}
	if(regexec(&preg,buf,1,pmatch,0) != 0)
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

void pong_server(char *msg,bool is_use_ssl)
{
	msg[1]='O';

	if(is_use_ssl)
		SSL_write(ssl,msg,strlen(msg));
	else
		send(sockfd,msg,strlen(msg),0);

	free(msg);
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

void smbot_help(bool is_use_ssl)
{
	char *msg="smbot使用!+指令+参数,通过!list指令获取smbot的所有指令。任何一个指令在其指令后不跟参数的情况下均会给出该指令所对应的功能";

	if(is_use_ssl)
		SSL_write(ssl,msg,strlen(msg));
	else
		send(sockfd,msg,strlen(msg),0);
}
