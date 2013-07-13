#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

int main(int argc,char **argv)
{
	regex_t reg;
	regmatch_t pmatch[1];
	char err[200];
	int nmatch=1;
	int errcode;
	int index;

	errcode=regcomp(&reg,argv[1],0);
	if(errcode != 0)
	{
		regerror(errcode,&reg,err,sizeof(err)-1);
		regfree(&reg);
		printf("%s\n",err);

		return -1;
	}

	errcode=regexec(&reg,argv[2],nmatch,pmatch,0);
	if(errcode != 0)
	{
		regerror(errcode,&reg,err,sizeof(err)-1);
		regfree(&reg);
		printf("%s\n",err);

		return -2;
	}
	regfree(&reg);

	for(index=pmatch[0].rm_so;index != pmatch[0].rm_eo;++index)
		printf("%c",argv[2][index]);

	printf("\n");

	return 0;
}
