#include "mysock/mysock.h"

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
	printf("%s",buf);
	free(buf);

	return 0;
}
