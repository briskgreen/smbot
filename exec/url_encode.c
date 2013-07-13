#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *url_encode(char *string)
{
	char *res;
	int len;
	int i,j;

	len=strlen(string)*3;
	res=malloc(len+1);
	bzero(res,len+1);

	for(i=0,j=0;string[i];++i,j+=3)
		sprintf(res+j,"%%%02x",(unsigned char)string[i]);

	return res;
}

int main(int argc,char **argv)
{
	printf("%s\n",url_encode(argv[1]));

	return 0;
}
