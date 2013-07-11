#include "youku.h"

char *get_youku_url(char *msg)
{
	int sockfd;
	char temp[512]={0};
	char *get="GET /search_video/q_";
	char *host="Host: www.soku.com\r\n";
	char *accept="Accept: */*\r\n";
	char *connection="Connection: close\r\n\r\n";
	char *buf;
	int i,j;
	int len;

	for(i=0;msg[i];++i)
		if(msg[i] == '!' && msg[i+1] == 'y' && msg[i+2] == 'o' && msg[i+3] == 'u' && msg[i+4] == 'k' && msg[i+5] == 'u')
			break;
	if(msg[i+6] == '\0' || msg[i+6] == '\n' || msg[i+6] != ' ')
		return "youku <target>\r\n";


	for(i+=7,j=0;msg[i];++i,++j)
		if(msg[i] == '\r' || msg[i] == '\n')
			break;
		else
			temp[j]=msg[i];

	buf=url_encode(temp);
	bzero(temp,sizeof(temp));
	strncpy(temp,buf,strlen(buf));
	len=strlen(get)+strlen(buf)+strlen(" HTTP/1.1\r\n");
	free(buf);

	buf=malloc(len+1);
	bzero(buf,len+1);
	strncpy(buf,get,strlen(get));
	strncat(buf,temp,strlen(temp));
	strncat(buf," HTTP/1.1\r\n",strlen(" HTTP/1.1\r\n"));

	sockfd=tcp_conect("www.soku.com",80);
	if(sockfd == -1)
		return "Sorry,无法连接远程服务器!\r\n";

	send(sockfd,buf,strlen(buf),0);
	send(sockfd,host,strlen(host),0);
	send(sockfd,accept,strlen(accept),0);
	send(sockfd,connection,strlen(connection),0);

	free(buf);

	while(buf=read_line(sockfd))
	{
		if(strstr(buf,"<a title="))
			break;

		free(buf);
	}

	if(buf == NULL)
		return "Sorry,no result found!\r\n";
	bzero(temp,sizeof(temp));
	for(i=0;buf[i];++i)
		if(buf[i] == 'h' && buf[i+1] == 'r' && buf[i+2] == 'e' && buf[i+3] == 'f')
			break;
	for(i+=6,j=0;buf[i];++i,++j)
		if(buf[i] == '"')
			break;
		else
			temp[j]=buf[i];
	free(buf);
	buf=malloc(strlen(temp)+3);
	bzero(buf,strlen(temp)+3);
	strncpy(buf,temp,strlen(temp));
	strncat(buf,"\r\n",strlen("\r\n"));

	return buf;
}
