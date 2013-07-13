#include "bt.h"

char *get_bt_magnet(char *buf)
{
	char *temp;
	char res[1024]={0};
	int pipefd[2];

	pipe(pipefd);

	if(fork()==0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/bt.sh","bt",buf,NULL);
	}

	close(pipefd[1]);

	read(pipefd[0],res,sizeof(res));
	temp=malloc(strlen(res)+2);
	bzero(temp,strlen(res)+2);
	strncpy(temp,res,strlen(res));
	strncat(temp,"\n",strlen("\n"));

	return temp;
}
