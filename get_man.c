#include "get_man.h"

char *get_man_url(char *msg)
{
	char *buf;
	char arg[2][20]={0};
	char *content_length="Content-Length: ";
	char *temp;
	char s_len[10];
	int sockfd;
	int len;
	int flags=0;
	int i,j;

	len=strlen("term=&section=&submitted=1");
	for(i=0;msg[i];++i)
		if(msg[i] == '!' && msg[i+1] == 'm' && msg[i+2] == 'a' && msg[i+3] == 'n')
			break;
	if(msg[i+4] == '\0'|| msg[i+4] == '\n' || msg[i+4] == '\r' || msg[i+4] != ' ')
		return "man <target>或者man [1-9] <target>\r\n";
	buf=msg+i+4;
	for(i=0;buf[i];++i)
		if(buf[i] == ' ')
			break;
	if(buf[i+1] >= '0' && buf[i+1] <= '9')
		flags=1;
	if(flags)
	{
		if(buf[i+2] != ' ')
			return "参数错误!\r\n";
		else
		{
			arg[0][0]=buf[i+1];
			arg[0][1]='\0';
		}

		for(buf=buf+3,i=0;buf[i];++i)
			if(buf[i] ==' ' || buf[i] == '\r')
				break;
		buf[i]='\0';
		strncpy(arg[1],buf,strlen(buf));
	}
	else
	{
		strncpy(arg[0],"-1",strlen("-1"));
		for(buf=buf+1,i=0;buf[i];++i)
			if(buf[i] == ' ' || buf[i] == '\r')
				break;
		buf[i]='\0';
		strncpy(arg[1],buf,strlen(buf));
	}

	len=len+strlen(arg[0])+strlen(arg[1]);
	sprintf(s_len,"%d",len);

	buf=malloc(len+5);
	bzero(buf,len+5);
	strncpy(buf,"term=",strlen("term="));
	strncat(buf,arg[1],strlen(arg[1]));
	strncat(buf,"&section=",strlen("&section="));
	strncat(buf,arg[0],strlen(arg[0]));
	strncat(buf,"&submitted=1",strlen("&submitted=1"));
	strncat(buf,"\r\n\r\n",strlen("\r\n\r\n"));

	temp=malloc(strlen(content_length)+strlen("\r\n\r\n")+strlen(s_len)+1);
	bzero(temp,strlen(content_length)+strlen("\r\n\r\n")+strlen(s_len)+1);
	strncpy(temp,content_length,strlen(content_length));
	strncat(temp,s_len,strlen(s_len));
	strncat(temp,"\r\n\r\n",strlen("\r\n\r\n"));

	sockfd=tcp_conect("www.linuxmanpages.com",80);
	if(sockfd == -1)
		return "Sorry,连接远程服务器失败!\r\n";
	send(sockfd,POST,strlen(POST),0);
	send(sockfd,HOST,strlen(HOST),0);
	send(sockfd,CONNECTION,strlen(CONNECTION),0);
	send(sockfd,CONTENT_TYPE,strlen(CONTENT_TYPE),0);
	send(sockfd,temp,strlen(temp),0);
	send(sockfd,buf,strlen(buf),0);

	free(buf);
	buf=NULL;
	free(temp);
	temp=NULL;

	while(buf=read_line(sockfd))
	{
		if(strstr(buf,"location: "))
			break;

		if(strstr(buf,"Sorry"))
		{
			free(buf);
			buf=NULL;
			
			return "Sorry,no result found!\r\n";
		}

		free(buf);
		buf=NULL;
	}

	close(sockfd);

	if(buf == NULL)
		return "Sorry,no result found!\r\n";

	temp=malloc(strlen("http://www.linuxmanpages.com")+strlen(buf)-strlen("location: ")+3);
	bzero(temp,strlen("http://www.linuxmanpages.com")+strlen(buf)-strlen("location: ")+3);

	buf=buf+10;
	strncpy(temp,"http://www.linuxmanpages.com",strlen("http://www.linuxmanpages.com"));
	strncat(temp,buf,strlen(buf));
	strncat(temp,"\r\n",strlen("\r\n"));

	free(buf-10);
	buf=NULL;
	return temp;
}

