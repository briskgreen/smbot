#include "google.h"

char *get_google_result(char *word)
{
	int len;
	int pipefd[2];
	char *res;
	char temp[1024]={0};

	pipe(pipefd);
	for(len=0;word[len];++len)
		if(word[len] == ' ')
			word[len]='+';

	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/google.sh","google",word,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,sizeof(temp));
	len=strlen(temp);
	res=malloc(len+1);
	bzero(res,len+1);
	strcpy(res,temp);

	return res;
}
