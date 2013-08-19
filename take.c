#include "mysock/mysock.h"

unsigned int unicode_len(const char *str)
{
	unsigned int len;
	int i;

	for(i=0,len=0;str[i];++i,++len)
	{
		if(str[i] == '\\' && str[i+1] == 'u')
			i+=5;
	}

	return len;
}

char *unicode_to_utf(const char *str)
{
	char *temp;
	char *res;
	char *buf;
	int len;
	int i,j;

	len=unicode_len(str);
	len*=2;
	temp=malloc(len+1);
	res=malloc(len*2);
	bzero(res,len*2);
	bzero(temp,len+1);

	for(i=0,j=0;str[i];++i,++j)
	{
		if(str[i] == '\\' && str[i+1] == 'u')
		{
			buf=strnstr(str+i+4,2);
			temp[j]=htoi(buf);
			free(buf);
			buf=strnstr(str+i+2,2);
			temp[++j]=htoi(buf);
			free(buf);
			i+=5;
		}
		else
			temp[j]=str[i];
	}

	to_iconv("UNICODE//","UTF-8//IGNORE",temp,len,res,len*2);
	free(temp);

	return res;
}

int main(int argc,char **argv)
{
	char *res;
	char *buf;
	int i;

	for(i=0;argv[1][i];++i)
		if(argv[1][i] == ' ')
			argv[1][i]='+';

	buf=string_add("http://xiaofengrobot.sinaapp.com/web.php?callback=jQuery191041205509454157474_1376842442554&para=%s&_=1376842442555",argv[1]);
	res=http_get_simple(buf,80);
	free(buf);

	buf=match_string("test(\".[^\"]*",res);
	free(res);
	printf("%s\n",buf);
	res=unicode_to_utf(buf+6);
	free(buf);
	printf("%s\n",res);
	free(res);

	return 0;
}
