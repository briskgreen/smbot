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

#define MEM_SIZE 80

typedef struct sockaddr SA;
typedef struct sockaddr_in SA_IN;

void error_quit(const char *msg);

int Socket(int domain,int type,int prot);

void Bind(int sockfd,SA_IN *server_addr);

void Listen(int sockfd,int backlog);

int Accept(int sockfd,SA_IN *client_addr,socklen_t *size);

void Connect(int sockfd,SA_IN *server_addr);

void init_data_with_server(SA_IN *server_addr,int port);

void init_data_with_client(SA_IN *server_addr,char *host,int port);

char *read_line(int sockfd);

int tcp_conect(char *url,int port);

char *url_encode(char *string);

#endif
