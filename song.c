#include "song.h"

char *get_song_url(char *song)
{
	int pipefd[2];
	char *res;
	char temp[512]={0};
	int len;

	pipe(pipefd);
	for(len=0;song[len];++len)
		if(song[len] == ' ')
			song[len]='+';

	if(fork() == 0)
	{
		close(pipefd[0]);
		
		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/song.sh","song",song,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,sizeof(temp));
	len=strlen(temp);
	res=malloc(len+1);
	bzero(res,len+1);
	strcpy(res,temp);

	return res;
}
