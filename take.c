#include "mysock/mysock.h"

int main(int argc,char **argv)
{
	char *res;
	char *buf;
	char temp[512];
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

	if(strreplace(res,"<br>","",temp,511) != -1)
		printf("%s\n",temp);
	else
		printf("%s\n",res);
	free(res);

	return 0;
}
