#include "../mysock/mysock.h"

int main(int argc,char **argv)
{
	printf("%s",url_encode(argv[1]));

	return 0;
}
