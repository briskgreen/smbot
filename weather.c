#include "weather.h"

char *get_weather(char *url)
{
	int pipefd[2];
	char *res;
	char temp[2048]={0};

	pipe(pipefd);

	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/weather.sh","weather",url,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,sizeof(temp));
	res=malloc(strlen(temp)+2);
	bzero(res,strlen(temp)+2);
	sprintf(res,"%s\n",temp);

	return res;
}
