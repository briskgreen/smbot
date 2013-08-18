#include <stdio.h>
#include "mysock/mysock.h"

int main(void)
{
	char temp[512]={0};
	char *t="\u8c22\u8c22\uff0c\u4f60\u4e5f\u597d\u554a";
	char *res="\x22\x8c\x22\x8c\x0c\xff\x60\x4f\x5f\x4e\x7d\x59\x4a\x55";

	printf("%s\n",t);
	to_iconv("UNICODE//","UTF-8//",res,strlen(res),temp,512);
	printf("%s\n",temp);

	return 0;
}
