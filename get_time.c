#include "get_time.h"

char *get_time(void)
{
	time_t t;

	t=time(NULL);

	return ctime(&t);
}
