#include "stack.h"

char *get_stack_over(char *ques)
{
	char *res;
	char *url="http://stackoverflow.com";
	char temp[1024]={0};
	int i;
	int pipefd[2];

	pipe(pipefd);
	for(i=0;ques[i];++i)
		if(ques[i] == ' ')
			ques[i]='+';

	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/stack.sh","stack",ques,NULL);
	}

	close(pipefd[1]);

	read(pipefd[0],temp,sizeof(temp));
	i=strlen(url)+strlen(temp)+2;
	res=malloc(i);
	bzero(res,i);
	sprintf(res,"%s%s\n",url,temp);

	return res;
}
