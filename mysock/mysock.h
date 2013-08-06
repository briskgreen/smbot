#ifndef _MYSOCK_H
#define _MYSOCK_H

#include <stdio.h>
#include <stdlib.h>
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

#define MEM_SIZE 512
#define TRUE 1
#define FALSE 0

typedef struct sockaddr SA;
typedef struct sockaddr_in SA_IN;
typedef unsigned char bool;

void error_quit(const char *msg);

int Socket(int domain,int type,int prot);

void Bind(int sockfd,SA_IN *server_addr);

void Listen(int sockfd,int backlog);

int Accept(int sockfd,SA_IN *client_addr,socklen_t *size);

void Connect(int sockfd,SA_IN *server_addr);

void init_data_with_server(SA_IN *server_addr,int port);

void init_data_with_client(SA_IN *server_addr,char *host,int port);

char *read_line(int sockfd);

int tcp_connect(const char *url,unsigned int port);

char *url_encode(char *string);

void safe_free(void **buf);

unsigned char *url_decode(char *code);

bool tcp_is_established(int sockfd);

SSL *ssl_connect(const char *host,unsigned int port,
		const char *cafile,const char *capath);

void ssl_close(SSL *ssl);

char *ssl_read_line(SSL *ssl);

#endif
