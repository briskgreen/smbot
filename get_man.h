#ifndef _GET_MAN_H
#define _GET_MAN_H

#include "mysock/mysock.h"

#define POST "POST /search.php HTTP/1.1\r\n"
#define HOST "Host: www.linuxmanpages.com\r\n"
#define CONNECTION "Connection: close\r\n"
#define CONTENT_TYPE "Content-Type: application/x-www-form-urlencoded\r\n"

char *get_man_url(char *msg);

#endif
