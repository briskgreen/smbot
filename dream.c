#include "dream.h"

char *get_dream(char *word)
{
	char *res;
	char *temp[1024]={0};
	int pipefd[2];
	int len;

	pipe(pipefd);
	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/dream.sh","dream",word,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,sizeof(temp));
	len=strlen(temp);
	res=malloc(len+1);
	bzero(res,len+1);
	strcpy(res,temp);

	return res;
}
