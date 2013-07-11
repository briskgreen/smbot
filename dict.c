#include "dict.h"

char *dict(char *msg)
{
	char *buf;
	char temp[200]={0};
	char *get="GET /s?wd=";
	char *host="Host: dict.baidu.com\r\n";
	char *connection="Connection: close\r\n";
	char *content_type="Content-Type: */*\r\n\r\n";
	int sockfd;
	int len;
	int i,j;
	
	len=strlen(get)+strlen(" HTTP/1.1\r\n");
	for(i=0;msg[i];++i)
		if(msg[i] == '!' && msg[i+1] == 'd' && msg[i+2] == 'i' && msg[i+3] == 'c' && msg[i+4] == 't')
			break;
	if(msg[i+5] == '\0' || msg[i+5] == '\r' || msg[i+5] != ' ')
		return "dict <word>\r\n";

	for(i+=6,j=0;msg[i];++i,++j)
		if(msg[i] == ' ' || msg[i] == '\r')
			break;
		else
			temp[j]=msg[i];
	len+=strlen(temp);
	buf=malloc(len+1);
	bzero(buf,len+1);
	strncpy(buf,get,strlen(get));
	strncat(buf,temp,strlen(temp));
	strncat(buf," HTTP/1.1\r\n",strlen(" HTTP/1.1\r\n"));

	sockfd=tcp_conect("dict.baidu.com",80);
	if(sockfd == -1)
		return "Sorry,连接远程服务器失败!\r\n";
	send(sockfd,buf,strlen(buf),0);
	send(sockfd,host,strlen(host),0);
	send(sockfd,connection,strlen(connection),0);
	send(sockfd,content_type,strlen(content_type),0);

	free(buf);
	buf=NULL;

	while(buf=read_line(sockfd))
	{
		if(strstr(buf,"dict-en-simplemeans-word"))
			break;
		if(strstr(buf,"dict-en-simplemeans-english"))
			break;

		free(buf);
		buf=NULL;
	}
	close(sockfd);

	if(buf == NULL)
		return "Sorry,no result found!\r\n";
	bzero(temp,sizeof(temp));

	if(strstr(buf,"dict-en-simplemeans-word"))
	{
		for(i=0;buf[i];++i)
			if(buf[i] == 'w' && buf[i+1] == 'd' && buf[i+2] == '=')
				break;
		for(;buf[i];++i)
			if(buf[i] == '"' && buf[i+1] == '>')
				break;
		for(j=0,i+=2;buf[i];++i,++j)
			if(buf[i] == '<')
				break;
			else
				temp[j]=buf[i];
		free(buf);
		buf=NULL;

		buf=malloc(strlen(temp)+3);
		bzero(buf,strlen(temp)+3);
		strncpy(buf,temp,strlen(temp));
		strncat(buf,"\r\n",strlen("\r\n"));
		
		return buf;
	}

	if(strstr(buf,"dict-en-simplemeans-english"))
	{
		for(i=0;buf[i];++i)
			if(buf[i] == 's' && buf[i+1] == 'p' && buf[i+2] == 'a' && buf[i+3] == 'n')
				break;
		for(i+=5,j=0;buf[i];++i,++j)
			if(buf[i] == '<')
				break;
			else
				temp[j]=buf[i];

		free(buf);
		buf=NULL;

		buf=malloc(strlen(temp)+3);
		bzero(buf,strlen(temp)+3);
		strncpy(buf,temp,strlen(temp));
		strncat(buf,"\r\n",strlen("\r\n"));

		return buf;
	}
}

