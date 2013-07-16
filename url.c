#include "url.h"

char *get_url_encode(char *url)
{
	char *res;
	char *temp;
	int len;

	temp=url_encode(url);
	len=strlen(temp);
	res=malloc(len+1);
	bzero(res,len+1);
	sprintf(res,"%s\n",temp);
	safe_free(&temp);

	return res;
}

char *get_url_decode(char *code)
{
	char *res;
	char *temp;
	int len;

	temp=url_decode(code);
	len=strlen(temp);
	res=malloc(len+2);
	bzero(res,len+2);
	sprintf(res,"%s\n",temp);
	safe_free(&temp);

	return res;
}
