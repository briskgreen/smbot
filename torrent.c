#include "torrent.h"

char *torrent(char *msg)
{
	char *get="GET /search/";
	char *host="Host: www.torrentkitty.com\r\n";
	char *content_type="Content-Type: */*\r\n";
	char *connection="Connection: close\r\n\r\n";
	char *buf;
	char temp[1024]={0};
	int sockfd;
	int len;
	int i,j;

	for(i=0;msg[i];++i)
		if(msg[i] == '!' && msg[i+1] == 't' && msg[i+2] == 'o' && msg[i+3] == 'r' && msg[i+4] == 'r' && msg[i+5] == 'e' && msg[i+6] =='n' && msg[i+7] == 't')
			break;

	if(msg[i+8] == '\0' || msg[i+8] =='\r' || msg[i+8] != ' ')
		return "torrent <target>\r\n";

	len=strlen(get)+strlen(" HTTP/1.1\r\n");
	for(i+=9,j=0;msg[i];++i,++j)
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
	
	sockfd=tcp_conect("www.torrentkitty.com",80);
	if(sockfd == -1)
		return "Sorry,连接远程服务器失败!\r\n";
	send(sockfd,buf,strlen(buf),0);
	send(sockfd,host,strlen(host),0);
	send(sockfd,content_type,strlen(content_type),0);
	send(sockfd,connection,strlen(connection),0);

	free(buf);
	buf=NULL;

	while(buf=read_line(sockfd))
	{
		if(strstr(buf,"<tr><td class=\"name\">"))
			break;

		free(buf);
		buf=NULL;
	}
	close(sockfd);

	if(buf == NULL)
		return "Sorry,no result found!\r\n";

	for(i=0;buf[i];++i)
		if(buf[i] == 'h' && buf[i+1] == 'r' && buf[i+2] == 'e' && buf[i+3] == 'f')
			break;
	bzero(temp,sizeof(temp));
	for(i+=6,j=0;buf[i];++i,++j)
		if(buf[i] == '"')
			break;
		else
			temp[j]=buf[i];

	free(buf);
	buf=NULL;

	buf=malloc(strlen(temp)+1);
	bzero(buf,strlen(temp)+1);
	strncpy(buf,temp,strlen(temp));
	
	return buf;
}

char *get_magnet(char *url)
{
	char *host="Host: www.torrentkitty.com\r\n";
	char *connection="Connection: close\r\n";
	char *content_type="Content-Type: */*\r\n\r\n";
	char *buf;
	char *temp;
	int sockfd;
	int len;
	int i,j;

	len=(strlen("GET ")+strlen(url)+strlen(" HTTP/1.1\r\n"));
	buf=malloc(len+1);
	bzero(buf,len+1);

	strncpy(buf,"GET ",strlen("GET "));
	strncat(buf,url,strlen(url));
	strncat(buf," HTTP/1.1\r\n",strlen(" HTTP/1.1\r\n"));

	sockfd=tcp_conect("www.torrentkitty.com",80);
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
		if(strstr(buf,"<textarea readonly class=\"magnet-link\" onfocus=\"this.select()\">"))
			break;

		free(buf);
		buf=NULL;
	}
	close(sockfd);

	if(buf == NULL)
		return "Sorry,no result found!\r\n";

	for(i=0;buf[i];++i)
		if(buf[i] == ')' && buf[i+1] == '"' && buf[i+2] == '>')
			break;
	j=i+3;
	for(i+=3,len=0;buf[i];++i,++len)
		if(buf[i] == '<')
			break;

	temp=malloc(len+3);
	bzero(temp,len+3);
	for(i=0;buf[j];++j,--len,++i)
		if(buf[j] == '<')
			break;
		else
			temp[i]=buf[j];
	strncat(temp,"\r\n",strlen("\r\n"));
	
	free(buf);
	buf=NULL;

	return temp;
}

