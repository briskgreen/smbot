#include "joke.h"

char *get_joke(char *word)
{
	char *res;
	char temp[1024]={0};
	int pipefd[2];
	int len;

	pipe(pipefd);
	for(len=0;word[len];++len)
		if(word[len] == ' ')
			word[len]='+';

	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/joke.sh","joke",word,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,sizeof(temp));
	len=strlen(temp);
	res=malloc(len+2);
	bzero(res,len+2);
	sprintf(res,"%s\n",temp);
	//strcpy(res,temp);

	return res;
}
