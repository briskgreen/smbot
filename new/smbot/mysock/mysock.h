#ifndef _MYSOCK_H
#define _MYSOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <regex.h>
#include <openssl/ssl.h>
#include <iconv.h>

#define MEM_SIZE 512
#define TRUE 1
#define FALSE 0

typedef struct sockaddr SA;
typedef struct sockaddr_in SA_IN;
typedef unsigned char bool;

typedef struct http_head
{
	const char *element;
	struct http_head *next;
}HTTP;

void error_quit(const char *msg);

int Socket(int domain,int type,int prot);

void Bind(int sockfd,SA_IN *server_addr);

void Listen(int sockfd,int backlog);

int Accept(int sockfd,SA_IN *client_addr,socklen_t *size);

void Connect(int sockfd,SA_IN *server_addr);

void init_data_with_server(SA_IN *server_addr,unsigned int port);

void init_data_with_client(SA_IN *server_addr,
		const char *host,unsigned int port);

char *read_line(int sockfd);

int tcp_connect(const char *url,unsigned int port);

char *url_encode(char *string);

void safe_free(void **buf);

char *url_decode(char *code);

bool tcp_is_established(int sockfd);

SSL *ssl_connect(const char *host,unsigned int port,
		const char *cafile,const char *capath);

void ssl_close(SSL *ssl);

char *ssl_read_line(SSL *ssl);

char *read_all(int sockfd);

char *ssl_read_all(SSL *ssl);

HTTP *http_head_init(void);

void http_head_add(HTTP *http,const char *head);

int http_head_replace(HTTP *http,const char *replace,const char *head);

int http_head_out(HTTP *http,const char *out);

void http_head_clean(HTTP *http);

void http_head_destroy(HTTP *http);

void http_send(HTTP *http,int sockfd);

void https_send(HTTP *http,SSL *ssl);

char *http_perform(HTTP *http,const char *host,unsigned int port);

char *https_perform(HTTP *http,const char *host,unsigned int port,
		const char *cafile,const char *capath);

char *http_get_simple(const char *url,unsigned int port);

char *http_post_simple(const char *url,unsigned int port,
		const char *data);

char *https_get_simple(const char *url,unsigned int port);

char *https_post_simple(const char *url,unsigned int port,
		const char *data);

char *string_add(const char *format,...);

char *strnstr(const char *str,int len);

int to_iconv(const char *from,const char *to,char *in,
		int in_len,char *des,int des_len);

unsigned long htoi(char *s);

char *match_string(const char *reg,char *data);

char *unicode_to_utf(const char *str);

int strreplace(char *str,char *replace,char *des,char *res,int res_len);

#endif
