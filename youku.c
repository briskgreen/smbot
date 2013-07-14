#include "youku.h"

char *get_youku_url(char *msg)
{
	regex_t reg;
	regmatch_t pmatch[1];
	int sockfd;
	char temp[512]={0};
	char *get="GET /search_video/q_";
	char *host="Host: www.soku.com\n";
	char *accept="Accept: */*\n";
	char *connection="Connection: close\n\n";
	char *buf;
	char *prg="http://*.[^\"]*";
	int flags=0;
	int len;

	buf=url_encode(msg);
	sprintf(temp,"%s",buf);
	len=strlen(get)+strlen(buf)+strlen(" HTTP/1.1\n");
	safe_free(&buf);

	buf=malloc(len+1);
	bzero(buf,len+1);
	sprintf(buf,"%s%s HTTP/1.1\n",get,temp);

	sockfd=tcp_conect("www.soku.com",80);
	if(sockfd == -1)
		return "Sorry,无法连接远程服务器!\n";

	send(sockfd,buf,len,0);
	send(sockfd,host,strlen(host),0);
	send(sockfd,accept,strlen(accept),0);
	send(sockfd,connection,strlen(connection),0);

	safe_free(&buf);

	while(buf=read_line(sockfd))
	{
		if(strstr(buf,"<a title="))
			break;
		if(strstr(buf,"<div class=\"sk-not-match"))
		{
			flags=1;
			break;
		}

		safe_free(&buf);
	}
	close(sockfd);

	if(buf == NULL)
		return "Sorry,no result found!\n";
	if(flags)
		prg=">.[^<]*";
	if(regcomp(&reg,prg,0) != 0)
	{
		regfree(&reg);
		return "Sorry,no result found!\n";
	}
	if(regexec(&reg,buf,1,pmatch,0) != 0)
	{
		regfree(&reg);
		return "Sorry,no result found!\n";
	}

	regfree(&reg);
	bzero(temp,sizeof(temp));
	snprintf(temp,pmatch[0].rm_eo-pmatch[0].rm_so+1,"%s",buf+pmatch[0].rm_so);
	safe_free(&buf);
	buf=malloc(strlen(temp)+2);
	bzero(buf,strlen(temp)+2);
	sprintf(buf,"%s\n",temp);

	return buf;
}
