#include "torrent.h"

char *torrent(char *msg)
{
	regex_t reg;
	regmatch_t pmatch[1];
	char *get="GET /search/";
	char *host="Host: www.torrentkitty.com\r\n";
	char *content_type="Content-Type: */*\r\n";
	char *connection="Connection: close\r\n\r\n";
	char *buf;
	char temp[2048]={0};
	int sockfd;
	int len;

	len=strlen(get)+strlen(msg)+strlen(" HTTP/1.1\r\n");
	buf=malloc(len+1);
	bzero(buf,len+1);
	sprintf(buf,"%s%s HTTP/1.1\r\n",get,msg);
	
	sockfd=tcp_conect("www.torrentkitty.com",80);
	if(sockfd == -1)
		return "Sorry,连接远程服务器失败!\r\n";
	send(sockfd,buf,len,0);
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

	if(regcomp(&reg,"magnet:?xt=urn:btih:*.[^\"]*",0) != 0)
	{
		regfree(&reg);
		return "Sorry,no result found!\r\n";
	}
	if(regexec(&reg,buf,1,pmatch,0) != 0)
	{
		regfree(&reg);
		return "Sorry,no result found!\r\n";
	}

	regfree(&reg);
	snprintf(temp,pmatch[0].rm_eo-pmatch[0].rm_so+1,"%s\r\n",buf+pmatch[0].rm_so);
	free(buf);
	buf=NULL;

	buf=malloc(strlen(temp)+1);
	bzero(buf,strlen(temp)+1);
	strcpy(buf,temp);
	
	return buf;
}

