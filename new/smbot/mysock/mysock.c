#include "mysock.h"

void format_str(char *code);
int htoi(char *s);
unsigned int url_len(char *url);

SSL_CTX *ssl_ctx;
int mysock_sockfd;

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

void init_data_with_server(SA_IN *server_addr,unsigned int port)
{
	bzero(server_addr,sizeof(SA_IN));
	server_addr->sin_family=AF_INET;
	server_addr->sin_port=htons(port);
	server_addr->sin_addr.s_addr=INADDR_ANY;
}

void init_data_with_client(SA_IN *server_addr,
		const char *host,unsigned int port)
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
	char *res;
	char temp;
	int len=0;
	int n;
	int flags=1;

	if((res=malloc(MEM_SIZE)) == NULL)
		return NULL;

	while((n=recv(sockfd,&temp,sizeof(char),0)) > 0)
	{
		++len;
		if(len % (MEM_SIZE) == 0)
		{
			char *temp;

			temp=malloc(len);
			strncpy(temp,res,len);
			free(res);

			res=malloc(len+MEM_SIZE);
			strncpy(res,temp,len);
			free(temp);
		}

		res[len-1]=temp;
		flags=0;
		if(temp == '\n')
			break;
	}


	res[len-1]='\0';
	if(flags && n <= 0)
		return NULL;

	return res;
}

int tcp_connect(const char *url,unsigned int port)
{
	SA_IN server_addr;
	int sockfd;
	int i;

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
		sprintf(res+j,"%%%02x",(unsigned char)string[i]);

	return res;
}

void safe_free(void **buf)
{
	if(*buf == NULL)
		return;

	free(*buf);
	*buf=NULL;
}

void format_str(char *code)
{
	int i;

	for(i=0;code[i];++i)
	{
		if(code[i] == '%')
			continue;
		if(code[i] >= 'A' && code[i] <= 'F')
			code[i]+=32;
	}
}

int htoi(char *s)
{
	int i;
	int res;

	for(i=0,res=0;s[i];++i)
	{
		res*=16;
		if(s[i] >= 'a' && s[i] <= 'f')
			res+=s[i]-87;
		else
			res+=s[i]-'0';
	}

	return res;
}

unsigned int url_len(char *url)
{
	int len;
	int i;

	for(i=0,len=0;url[i];++i,++len)
	{
		if(url[i] == '%' && url[i+1] != '%' && url[i+2] != '%')
		{

			if(url[i+1] < '0' || url[i+2] < '0' || url[i+1] > 'f' || url[i+2] > 'f')
				continue;
			else
				i+=2;
		}

	}

	return len;
}

char *url_decode(char *code)
{
	unsigned int len;
	char *res;
	char temp[3]={0};
	int i,j;

	format_str(code);
	len=url_len(code);
	res=malloc(len+1);

	for(i=0,j=0;code[i];++i,++j)
	{
		if(code[i] == '%' && code[i+1] != '%' && code[i+2] != '%')
		{
			if(code[i+1] < '0' || code[i+2] < '0' || code[i+1] > 'f' || code[i+2] > 'f')
			{
				res[j]=code[i];
				continue;
			}

			snprintf(temp,3,"%s",code+i+1);
			temp[2]='\0';
			res[j]=htoi(temp);
			i+=2;
			continue;
		}

		res[j]=code[i];
	}

	res[len]='\0';

	return res;
}

bool tcp_is_established(int sockfd)
{
	int optval;
	int optlen;

	getsockopt(sockfd,SOL_SOCKET,SO_ERROR,(char *)&optval,&optlen);

	switch(optval)
	{
		case 0:
			return TRUE;
		default:
			return FALSE;
	}
}

SSL *ssl_connect(const char *host,unsigned int port,
		const char *cafile,const char *capath)
{
	SSL *ssl;

	if((mysock_sockfd=tcp_connect(host,port)) == -1)
		return NULL;

	SSL_library_init();
	if((ssl_ctx=SSL_CTX_new(SSLv23_client_method())) == NULL)
		return NULL;
	if(capath != NULL)
		if(!SSL_CTX_load_verify_locations(ssl_ctx,cafile,capath))
			return NULL;

	if((ssl=SSL_new(ssl_ctx)) == NULL)
		return NULL;
	if(SSL_set_fd(ssl,mysock_sockfd) == 0)
		return NULL;

	if(SSL_connect(ssl) <= 0)
		return NULL;

	return ssl;
}

void ssl_close(SSL *ssl)
{
	SSL_shutdown(ssl);
	close(mysock_sockfd);
	SSL_free(ssl);
	SSL_CTX_free(ssl_ctx);
}

char *ssl_read_line(SSL *ssl)
{
	char *res;
	char temp;
	int len=0;
	int n;
	int flags=1;

	if((res=malloc(MEM_SIZE)) == NULL)
		return NULL;

	while((n=SSL_read(ssl,&temp,sizeof(char))) > 0)
	{
		++len;
		if(len % MEM_SIZE == 0)
		{
			char *temp;

			temp=malloc(len);
			strncpy(temp,res,len);
			free(res);

			res=malloc(len+MEM_SIZE);
			strncpy(res,temp,len);
			free(temp);
		}

		res[len-1]=temp;
		flags=0;
		if(temp == '\n')
			break;
	}

	res[len-1]='\0';
	if(flags && n <= 0)
		return NULL;

	return res;
}

char *read_all(int sockfd)
{
	char *res;
	int size=MEM_SIZE;
	int n;
	int len=0;

	res=malloc(MEM_SIZE);
	if(res == NULL)
		return NULL;

	while((n=recv(sockfd,res+len,size,0)))
	{
		len+=n;

		if(n < size)
			size-=n;

		if(len % MEM_SIZE == 0)
		{
			char *temp;

			temp=malloc(len);
			strncpy(temp,res,len);
			free(res);

			res=malloc(len+MEM_SIZE);
			strncpy(res,temp,len);
			free(temp);

			size=MEM_SIZE;
		}
	}

	res[len]='\0';

	return res;
}

char *ssl_read_all(SSL *ssl)
{
	char *res;
	int size=MEM_SIZE;
	int len=0;
	int n;
	if((res=malloc(MEM_SIZE)) == NULL)
		return NULL;

	while((n=SSL_read(ssl,res+len,size)))
	{
		len+=n;

		if(n < size)
			size-=n;

		if(len % MEM_SIZE == 0)
		{
			char *temp;

			temp=malloc(len);
			strncpy(temp,res,len);
			free(res);

			res=malloc(len+MEM_SIZE);
			strncpy(res,temp,len);
			free(temp);

			size=MEM_SIZE;
		}
	}

	res[len]='\0';
	return res;
}

HTTP *http_head_init(void)
{
	HTTP *http;

	if((http=malloc(sizeof(HTTP))) == NULL)
		return NULL;

	http->element=NULL;
	http->next=NULL;

	return http;
}

void http_head_add(HTTP *http,const char *head)
{
	HTTP *node;

	while(http->next != NULL)
		http=http->next;

	node=malloc(sizeof(HTTP));
	node->element=head;
	node->next=NULL;
	http->next=node;
}

int http_head_replace(HTTP *http,const char *replace,const char *head)
{
	http=http->next;

	while(http != NULL)
	{
		if(strstr(http->element,replace))
		{
			http->element=head;
			return 0;
		}

		http=http->next;
	}

	return -1;
}

int http_head_out(HTTP *http,const char *out)
{
	while(http != NULL)
	{
		if(strstr(http->next->element,out))
		{
			free(http->next);
			http->next=http->next->next;
			return 0;
		}

		http=http->next;
	}

	return -1;
}

void http_head_clean(HTTP *http)
{
	HTTP *temp;

	temp=http->next;
	http->next=NULL;
	http->element=NULL;

	while(temp != NULL)
	{
		free(temp);
		temp=temp->next;
	}
}

void http_head_destroy(HTTP *http)
{
	while(http != NULL)
	{
		free(http);
		http=http->next;
	}
}

void http_send(HTTP *http,int sockfd)
{
	while(http->next != NULL)
	{
		http=http->next;

		send(sockfd,http->element,strlen(http->element),0);
	}
}

void https_send(HTTP *http,SSL *ssl)
{
	while(http->next != NULL)
	{
		http=http->next;

		SSL_write(ssl,http->element,strlen(http->element));
	}
}

char *http_perform(HTTP *http,const char *host,unsigned int port)
{
	int sockfd;
	char *res;

	if((sockfd=tcp_connect(host,port)) == -1)
		return NULL;

	http_send(http,sockfd);
	res=read_all(sockfd);
	close(sockfd);

	return res;
}

char *https_perform(HTTP *http,const char *host,unsigned int port,
		const char *cafile,const char *capath)
{
	SSL *ssl;
	char *res;

	if((ssl=ssl_connect(host,port,cafile,capath)) == NULL)
		return NULL;
	https_send(http,ssl);
	res=ssl_read_all(ssl);
	ssl_close(ssl);

	return res;
}

char *http_get_simple(const char *url,unsigned int port)
{
	int sockfd;
	char *res;
	char *accept="Accept: */*\n";
	char *connection="Connection: close\n\n";
	char *host;
	char *head;
	int host_len=0;
	int n=0;
	int i;

	if(strstr(url,"https://"))
		return NULL;
	if(strstr(url,"http://"))
		n=7;
	for(i=n;url[i];++i,++host_len)
		if(url[i]=='/')
			break;
	host=string_add("Host: %s\n",strnstr(url+n,host_len));
	head=string_add("GET %s HTTP/1.1\n",url+n+host_len);

	res=strnstr(host+6,-1);

	if((sockfd=tcp_connect(res,port)) == -1)
	{
		free(res);
		free(host);
		free(head);

		return NULL;
	}
	free(res);
	
	send(sockfd,head,strlen(head),0);
	send(sockfd,host,strlen(host),0);
	send(sockfd,accept,strlen(accept),0);
	send(sockfd,connection,strlen(connection),0);

	free(head);
	free(host);

	res=read_all(sockfd);
	close(sockfd);

	return res;
}

char *http_post_simple(const char *url,unsigned int port,
		const char *data)
{
	int sockfd;
	char *res;
	char *head;
	char *host;
	char *accept="Accept: */*\n";
	char *content_type="Content-Type: application/x-www-form-urlencoded\n";
	char *connection="Connection: close\n";
	char *content_length;
	int host_len=0;
	int n=0;
	int i;

	if(strstr(url,"https://"))
		return NULL;
	if(strstr(url,"http://"))
		n=7;
	for(i=n;url[i];++i,++host_len)
		if(url[i] == '/')
			break;

	content_length=string_add("Content-Length: %d\n\n",strlen(data));
	host=string_add("Host: %s\n",strnstr(url+n,host_len));
	head=string_add("POST %s HTTP/1.1\n",url+n+host_len);
	res=strnstr(host+6,-1);

	if((sockfd=tcp_connect(res,port)) == -1)
	{
		free(host);
		free(head);
		free(res);
		free(content_length);

		return NULL;
	}
	free(res);

	send(sockfd,head,strlen(head),0);
	send(sockfd,host,strlen(host),0);
	send(sockfd,accept,strlen(accept),0);
	send(sockfd,content_type,strlen(content_type),0);
	send(sockfd,connection,strlen(connection),0);
	send(sockfd,content_length,strlen(content_length),0);
	send(sockfd,data,strlen(data),0);

	free(head);
	free(host);
	free(content_length);

	res=read_all(sockfd);
	close(sockfd);

	return res;
}

char *https_get_simple(const char *url,unsigned int port)
{
	SSL *ssl;
	char *res;
	char *head;
	char *host;
	char *accept="Accept: */*\n";
	char *connection="Connection: close\n\n";
	int host_len=0;
	int n=0;
	int i;

	if(strstr(url,"http://"))
		return NULL;
	if(strstr(url,"https://"))
		n=8;
	for(i=n;url[i];++i,++host_len)
		if(url[i] == '/')
			break;

	host=string_add("Host: %s\n",strnstr(url+n,host_len));
	head=string_add("GET %s HTTP/1.1\n",url+n+host_len);
	res=strnstr(host+6,-1);

	if((ssl=ssl_connect(res,port,NULL,NULL)) == NULL)
	{
		free(res);
		free(host);
		free(head);

		return NULL;
	}
	free(res);

	SSL_write(ssl,head,strlen(head));
	SSL_write(ssl,host,strlen(host));
	SSL_write(ssl,accept,strlen(accept));
	SSL_write(ssl,connection,strlen(connection));

	free(head);
	free(host);

	res=ssl_read_all(ssl);
	ssl_close(ssl);

	return res;
}

char *https_post_simple(const char *url,unsigned int port,
		const char *data)
{
	SSL *ssl;
	char *res;
	char *head;
	char *host;
	char *accept="Accept: */*\n";
	char *connection="Connection: close\n";
	char *content_length;
	int host_len=0;
	int n=0;
	int i;

	if(strstr(url,"http://"))
		return NULL;
	if(strstr(url,"https://"))
		n=8;
	for(i=n;url[i];++i,++host_len)
		if(url[i] == '/')
			break;

	content_length=string_add("Content-Length: %d\n\n",strlen(data));
	host=string_add("Host: %s\n",strnstr(url+n,host_len));
	head=string_add("POST %s HTTP/1.1\n",url+n+host_len);
	res=strnstr(host+6,-1);

	if((ssl=ssl_connect(res,port,NULL,NULL)) == NULL)
	{
		free(content_length);
		free(host);
		free(head);
		free(res);

		return NULL;
	}
	free(res);

	SSL_write(ssl,head,strlen(head));
	SSL_write(ssl,host,strlen(host));
	SSL_write(ssl,accept,strlen(accept));
	SSL_write(ssl,connection,strlen(connection));
	SSL_write(ssl,content_length,strlen(content_length));
	SSL_write(ssl,data,strlen(data));

	free(head);
	free(host);
	free(content_length);

	res=ssl_read_all(ssl);
	return res;
}

char *string_add(const char *format,...)
{
	va_list arg_ptr;
	int len;
	FILE *fp;
	char *res;
	
	va_start(arg_ptr,format);

	if((fp=fopen("/dev/null","w")) == NULL)
		return NULL;
	len=vfprintf(fp,format,arg_ptr);
	fclose(fp);
	if(len < 0)
		return NULL;

	res=malloc(len+1);
	if(res == NULL)
		return NULL;
	vsnprintf(res,len+1,format,arg_ptr);

	va_end(arg_ptr);

	return res;
}

char *strnstr(const char *str,int len)
{
	char *res;

	if(len < 0)
		len=strlen(str)+len;

	res=malloc(len+1);
	if(res == NULL)
		return NULL;
	snprintf(res,len+1,str);

	return res;
}

int to_iconv(const char *from,const char *to,char *in,
		int in_len,char *des,int des_len)
{
	iconv_t cd;

	if((cd=iconv_open(to,from)) == (iconv_t)-1)
		return -1;

	if(iconv(cd,&in,&in_len,&des,&des_len) == -1)
		return -1;

	iconv_close(cd);

	return 0;
}

