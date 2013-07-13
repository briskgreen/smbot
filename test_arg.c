#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <sys/types.h>

char *get_arg(char *buf,char *prg,char *des)
{
	regex_t preg;
	regmatch_t pmatch[1];
	int nmatch=1;
	char *res;
	int index;
	int len;
	char temp;

	if(regcomp(&preg,prg,0) != 0)
	{
		printf("%s\n",prg);
		return NULL;
	}
	if(regexec(&preg,buf,nmatch,pmatch,0) !=0)
	{
		printf("%s\n",buf);
		regfree(&preg);
		return NULL;
	}
	regfree(&preg);

	index=pmatch[0].rm_eo;
	temp=buf[index];
	if(temp != ' ' && temp == '\n')
		return des;
	else if(temp != ' ' && temp != '\n')
	{
		printf("%c %d\n",temp,temp);
		return NULL;
	}
	for(index+=2,len=0;buf[index];++len,++index);
	res=malloc(len+1);
	bzero(res,len+1);
	strncpy(res,buf+pmatch[0].rm_eo+2,len);

	return res;
}

int main(int argc,char **argv)
{
	char *res;

	res=get_arg("!man fjsdk fjdsakf\n","\\!man","No");

	if(res == NULL)
	{
		printf("NULL\n");
		return -1;
	}
	printf("%s",res);

	return 0;
}
