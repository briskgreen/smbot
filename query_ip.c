#include "query_ip.h"

char *query_ip(char *ip)
{
	int pipefd[2];
	char temp[200]={0};
	char *buf;
	int i,j;

	if(pipe(pipefd) == -1)
		return "创建管道失败!\n";

	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("exec/query_ip_addr","query_ip_addr",ip,NULL);
	}

	close(pipefd[1]);

	read(pipefd[0],temp,sizeof(temp));
	buf=malloc(strlen(temp)+2);
	bzero(buf,strlen(temp)+2);
	sprintf(buf,"%s\n",temp);

	return buf;
}

