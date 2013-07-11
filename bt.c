#include "bt.h"

char *get_bt_magnet(char *buf)
{
	char res[1024]={0};
	char *temp;
	int i,j;
	int pipefd[2];

	for(i=0;buf[i];++i)
		if(buf[i] == '!' && buf[i+1] == 'b' && buf[i+2] == 't')
			break;
	if(buf[i+3] == '\r' || buf[i+3] == '\n' || buf[i+3] != ' ')
		return "bt <target>\r\n";
	for(i+=4,j=0;buf[i];++i,++j)
		if(buf[i] == '\r' || buf[i] == '\n')
			break;
		else 
			res[j]=buf[i];

	pipe(pipefd);

	if(fork()==0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("sh/bt.sh","bt",res,NULL);
	}

	close(pipefd[1]);
	bzero(res,sizeof(res));

	read(pipefd[0],res,sizeof(res));
	temp=malloc(strlen(res)+3);
	bzero(temp,strlen(res)+3);
	strncpy(temp,res,strlen(res));
	strncat(temp,"\r\n",strlen("\r\n"));

	return temp;
}
