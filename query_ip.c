#include "query_ip.h"

char *query_ip(char *ip)
{
	int pipefd[2];
	char temp[200]={0};
	char *buf;
	int i,j;

	if(pipe(pipefd) == -1)
		return "创建管道失败!\r\n";

	for(i=0;ip[i];++i)
		if(ip[i] == '!' && ip[i+1] == 'i' && ip[i+2] == 'p')
			break;
	if(ip[i+3] == '\0' || ip[i+3] =='\r' || ip[i+3] != ' ')
		return "ip <ip address>\r\n";
	for(i+=4,j=0;ip[i];++i,++j)
		if(ip[i] == ' ' || ip[i] == '\r')
			break;
		else
			temp[j]=ip[i];

	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("exec/query_ip_addr","query_ip_addr",temp,NULL);
	}

	close(pipefd[1]);
	bzero(temp,sizeof(temp));

	read(pipefd[0],temp,sizeof(temp));
	buf=malloc(strlen(temp)+3);
	bzero(buf,strlen(temp)+3);
	strncpy(buf,temp,strlen(temp));
	strncat(buf,"\r\n",strlen("\r\n"));

	return buf;
}

