#include "checkid.h"

char *check_id_card(char *id)
{
	char *res;
	char temp[512]={0};
	int pipefd[2];

	pipe(pipefd);
	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("exec/id_card_test","id_card_test",id,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,sizeof(temp));
	res=malloc(strlen(temp)+1);
	bzero(res,strlen(temp)+1);
	strcpy(res,temp);

	return res;
}
