#include <stdio.h>
#include <regex.h>
#include <sys/types.h>

int main(int argc,char **argv)
{
	regex_t preg;
	regmatch_t pmatch[1];
	int nmatch=1;
	char err[256]={0};
	int errcode;
	int i;

	errcode=regcomp(&preg,"magnet:?xt=urn:btih:*.[^\"]*",0);

	if(errcode != 0)
	{
		regerror(errcode,&preg,err,sizeof(err)-1);

		printf("%s\n",err);

		return -1;
	}

	errcode=regexec(&preg,argv[1],nmatch,pmatch,0);
	if(errcode != 0)
	{
		regerror(errcode,&preg,err,sizeof(err)-1);

		printf("%s\n",err);

		return -2;
	}

	for(i=pmatch[0].rm_so;i != pmatch[0].rm_eo;++i)
		printf("%c",argv[1][i]);

	regfree(&preg);

	printf("\n");
	return 0;
}	
