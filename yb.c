#include "yb.h"

char *get_yb_code(char *buf)
{
	char *temp;
	char res[100]={0};
	int pipefd[2];

	pipe(pipefd);

	if(fork()==0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/yb.sh","yb",buf,NULL);
	}

	close(pipefd[1]);

	read(pipefd[0],res,sizeof(res));
	temp=malloc(strlen(res)+2);
	bzero(temp,strlen(res)+2);
	sprintf(temp,"%s\n",res);

	return temp;
}
