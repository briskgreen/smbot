#include "dict.h"

char *dict(char *msg)
{
	regex_t reg;
	regmatch_t pmatch[1];
	char *buf;
	char temp[300]={0};
	char *get="GET /s?wd=";
	char *host="Host: dict.baidu.com\r\n";
	char *connection="Connection: close\r\n";
	char *content_type="Content-Type: */*\r\n\r\n";
	int sockfd;
	int len;
	int i,j;
	
	len=strlen(get)+strlen(" HTTP/1.1\r\n")+strlen(msg);
	buf=malloc(len+1);
	bzero(buf,len+1);
	sprintf(buf,"%s%s HTTP/1.1\r\n",get,msg);

	sockfd=tcp_conect("dict.baidu.com",80);
	if(sockfd == -1)
		return "Sorry,连接远程服务器失败!\r\n";
	send(sockfd,buf,len,0);
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

	if(strstr(buf,"dict-en-simplemeans-word"))
	{
		if(regcomp(&reg,"wd=*.[^\"]*",0) != 0)
		{
			regfree(&reg);
			return "Sorry,no result found!\r\n";
		}
		if(regexec(&reg,buf,1,pmatch,0) != 0)
		{
			regfree(&reg);
			return "Sorry,no result found!\r\n";
		}

		snprintf(temp,pmatch[0].rm_eo-pmatch[0].rm_so-2,"%s",buf+pmatch[0].rm_so+3);
	}

	if(strstr(buf,"dict-en-simplemeans-english"))
	{
		if(regcomp(&reg,"<span>*.[^<]*",0) != 0)
		{
			regfree(&reg);
			return "Sorry,no result found!\r\n";
		}
		if(regexec(&reg,buf,1,pmatch,0) != 0)
		{
			regfree(&reg);
			return "Sorry,no result found!\r\n";
		}

		snprintf(temp,pmatch[0].rm_eo-pmatch[0].rm_so-5,"%s",buf+pmatch[0].rm_so+6);
	}
	regfree(&reg);

	free(buf);
	buf=malloc(strlen(temp)+3);
	bzero(buf,strlen(temp)+3);
	sprintf(buf,"%s\r\n",temp);

	return buf;
}

