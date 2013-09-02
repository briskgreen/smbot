#include "../mysock/mysock.h"

typedef char BOOL;

#define TRUE 1
#define FALSE 0

BOOL is_ipv6(const char *ip)
{
	int i;

	for(i=0;ip[i];++i)
	{
		if(ip[i] == ':')
			return TRUE;
		else if(ip[i] == '.')
			return FALSE;
	}

	return -1;
}

int main(int argc,char **argv)
{
	char *res;
	char *buf;
	BOOL flags;

	flags=is_ipv6(argv[1]);
	if(flags == -1)
	{
		printf("Invalid ip address\n");
		return -1;
	}

	if(flags)
		buf=string_add("http://ipv6.ipv6home.cn/ip/?ip=%s",argv[1]);
	else
		buf=string_add("http://ip.chinaz.com/?IP=%s",argv[1]);
	while((res=http_get_simple(buf,80)))
		if(res[0] <= '\0' || res == NULL)
			continue;
		else
			break;
	free(buf);
	if(flags)
		buf=match_string("IPv6地址: .[^<]*",res);
	else
		buf=match_string("==>> [^0-9].[^<]*",res);

	free(res);
	if(buf == NULL)
		printf("Sorry,no result found!\n");
	else
	{
		if(flags)
			printf("%s\n",buf+strlen("IPv6地址: "));
		else
			printf("%s\n",buf+5);
		
		free(buf);
	}

	return 0;
}
