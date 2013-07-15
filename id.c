#include "id.h"

char *get_id_info(char *buf)
{
	char *res;
	char temp[1024]={0};
	int pipefd[2];

	pipe(pipefd);
	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/id.sh","id",buf,NULL);
	}

	close(pipefd[1]);

	read(pipefd[0],temp,sizeof(temp));
	res=malloc(strlen(temp)+1);
	bzero(res,strlen(temp)+1);
	strcpy(res,temp);

	return res;
}
