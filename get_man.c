#include "get_man.h"

char *get_man_url(char *msg)
{
	char *buf;
	char *res;
	int sockfd;
	int len;
	char content_length[100]={0};
	char arg[2][20]={0};
	int i;
	int flag=0;

	for(i=0;msg[i];++i)
		if(msg[i] == ' ')
		{
			flag=1;
			break;
		}

	if(flag)
	{
		sscanf(msg,"%s",arg[0]);
		sscanf(msg+i+1,"%s",arg[1]);
	}
	else
	{
		strcpy(arg[0],"-1");
		strncpy(arg[1],msg,strlen(msg));
	}

	len=strlen("term=&section=&submitted=1")+strlen(arg[0])+strlen(arg[1]);
	buf=malloc(len+2);
	bzero(buf,len+2);
	sprintf(buf,"term=%s&section=%s&submitted=1\n",arg[1],arg[0]);
	sprintf(content_length,"Content-Length: %d\n\n",len);

	sockfd=tcp_conect("www.linuxmanpages.com",80);
	if(sockfd == -1)
		return "Sorry,连接远程服务器失败!\n";

	send(sockfd,POST,strlen(POST),0);
	send(sockfd,HOST,strlen(HOST),0);
	send(sockfd,CONNECTION,strlen(CONNECTION),0);
	send(sockfd,CONTENT_TYPE,strlen(CONTENT_TYPE),0);
	send(sockfd,content_length,strlen(content_length),0);
	send(sockfd,buf,len+1,0);

	safe_free(&buf);

	while(buf=read_line(sockfd))
	{
		if(strstr(buf,"location: "))
			break;
		if(strstr(buf,"Sorry"))
		{
			safe_free(&buf);
			return "Sorry,no result found!\n";
		}

		safe_free(&buf);
	}
	
	close(sockfd);
	if(buf == NULL)
		return "Sorry,no result found!\n";
	len=strlen("http://www.linuxmanpages.com")+strlen(buf)-strlen("location: ")+2;
	res=malloc(len);
	bzero(res,len);
	sprintf(res,"http://www.linuxmanpages.com%s\n",buf+10);

	safe_free(&buf);
	return res;
}

