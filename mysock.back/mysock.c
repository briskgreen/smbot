#include "mysock.h"

void error_quit(const char *msg)
{
	perror(msg);
	exit(-1);
}

int Socket(int domain,int type,int prot)
{
	int sockfd;

	if((sockfd=socket(domain,type,prot))==-1)
		error_quit("Socket");

	return sockfd;
}

void Bind(int sockfd,SA_IN *server_addr)
{
	if(bind(sockfd,(SA *)server_addr,sizeof(SA_IN))==-1)
		error_quit("Bind");
}

void Listen(int sockfd,int backlog)
{
	if(listen(sockfd,backlog)==-1)
		error_quit("Listen");
}

int Accept(int sockfd,SA_IN *client_addr,socklen_t *size)
{
	int new_sockfd;

	if((new_sockfd=accept(sockfd,(SA *)client_addr,size))==-1)
		error_quit("Accept");

	return new_sockfd;
}

void Connect(int sockfd,SA_IN *server_addr)
{
	if(connect(sockfd,(SA *)server_addr,sizeof(SA_IN))==-1)
		error_quit("Connect");
}

void init_data_with_server(SA_IN *server_addr,int port)
{
	bzero(server_addr,sizeof(SA_IN));
	server_addr->sin_family=AF_INET;
	server_addr->sin_port=htons(port);
	server_addr->sin_addr.s_addr=INADDR_ANY;
}

void init_data_with_client(SA_IN *server_addr,char *host,int port)
{
	struct hostent *hostname;

	if((hostname=gethostbyname(host))==NULL)
	{
		herror("gethostbyname");
		exit(-1);
	}

	bzero(server_addr,sizeof(SA_IN));
	server_addr->sin_family=AF_INET;
	server_addr->sin_port=htons(port);
	server_addr->sin_addr.s_addr=inet_addr(inet_ntoa(*(struct in_addr *)hostname->h_addr_list[0]));
}

char *read_line(int sockfd)
{
	char *buf;
	char temp;
	int k=0;
	int i=0;
	int j=1;

	if((buf=malloc(MEM_SIZE)) == NULL)
		return NULL;

	while(1)
	{
		if(recv(sockfd,&temp,sizeof(char),0) <= 0)
		{
			++k;

			if(k % 5)
				return NULL;
			else
				continue;
		}

		if((i+1) % (MEM_SIZE-2) == 0)
		{
			char *t;

			k=0;
			while((t=malloc(MEM_SIZE*j)) == NULL)
			{
				++k;

				if(k % 5 ==0)
					return NULL;
			}
			strncpy(t,buf,i);
			free(buf);

			++j;	
			while((buf=malloc(MEM_SIZE*j)) == NULL)
			{
				++k;

				if(k % 5 ==0)
					return NULL;
			}
			strncpy(buf,t,i);
			free(t);
		}

		buf[i] = temp;
		if(temp == '\n' || temp == '\r')
			break;
		++i;
		k=0;
	}

	buf[i+1]='\0';

	return buf;
}

int tcp_conect(char *url,int port)
{
	SA_IN server_addr;
	int sockfd;

	sockfd=Socket(AF_INET,SOCK_STREAM,0);
	init_data_with_client(&server_addr,url,port);
	if(connect(sockfd,(SA *)&server_addr,sizeof(SA_IN)) == -1)
		return -1;

	return sockfd;
}

char *url_encode(char *string)
{
	char *res;
	int len;
	int i,j;

	len=strlen(string)*3;
	res=malloc(len+1);
	bzero(res,len+1);

	for(i=0,j=0;string[i];++i,j+=3)
		sprintf(res+j,"%%%x",(unsigned char)string[i]);

	return res;
}
