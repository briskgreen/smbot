#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error_quit(const char *msg)
{
	printf("%s\n",msg);
	exit(1);
}

int add_mux(int *id,int *mask)
{
	int i;
	int result=0;

	for(i=0;i<17;i++)
		result+=id[i]*mask[i];

	return result;
}

void get_id(int *id,char *num)
{
	int i;

	for(i=0;num[i];i++)
	{
		if(num[i]=='X'||num[i]=='x')
		{
			id[i]=10;
			continue;
		}

		id[i]=num[i]-48;
	}
}

void if_id_right(int *id)
{
	int n=18;
	int i;

	for(i=0;i<17;i++)
		if(id[i]<0||id[i]>9)
			error_quit("错误的身份证号码!");

	if(id[i]>10)
		error_quit("错误的身份证号码!");
}
int main(int argc,char **argv)
{
	int id[18];
	int mask[17]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2};
	int res[11]={1,0,10,9,8,7,6,5,4,3,2};
	int result;

	if(argc!=2)
		error_quit("Argument Error!");

	get_id(id,argv[1]);
	if(strlen(argv[1])!=18)
		error_quit("本校验程序只接受18位身份证号码!");

	if_id_right(id);
	printf("ID:%s:",argv[1]);

	result=add_mux(id,mask);
	result%=11;
	if(result[res]==id[17])
		printf("校验成功!\n");
	else
		printf("校验失败，最后一位可能是%d\n",result[res]);

	return 0;
}
