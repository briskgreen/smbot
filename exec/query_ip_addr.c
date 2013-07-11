#include "../mysock/mysock.h"
#include <iconv.h>

typedef int BOOL;

#define TRUE 1
#define FALSE 0
#define H_IPV_4 "ip38.com"
#define H_IPV_6 "ipv6.ipv6home.cn"
#define PORT 80
#define error_return(msg)\
{\
	printf("%s\n",msg);\
	return -1;\
}

BOOL is_ipv6(const char *ip)
{
	int i;

	for(i=0;ip[i];++i)
	{
		if(ip[i] == ':')
			return TRUE;
		else if(ip[i] == '.')
			return FALSE;
	}

	return -1;
}

void send_http_get_request(int sockfd,char *ip,BOOL flage)
{
	char *head;
	char *content_type="Content-Type: text/html\n";
	char *host;
	char *connection="Connection: close\n\n";

	if(flage)
	{
		head=malloc(strlen("GET /ip/?ip=")+strlen(ip)+strlen(" HTTP/1.1\n"));
		strcpy(head,"GET /ip/?ip=");
		host="Host: ipv6.ipv6home.cn\n";
	}
	else
	{
		head=malloc(strlen("GET /index.php?ip=")+strlen(ip)+strlen(" HTTP/1.1\n"));
		strcpy(head,"GET /index.php?ip=");
		host="Host: ip38.com\n";
	}
	
	strncat(head,ip,strlen(ip));
	strncat(head," HTTP/1.1\n",strlen(" HTTP/1.1\n"));

	send(sockfd,head,strlen(head),0);
	send(sockfd,host,strlen(host),0);
	send(sockfd,content_type,strlen(content_type),0);
	send(sockfd,connection,strlen(connection),0);

	free(head);
}

int parse_ipv6(char *buf)
{
	if(strstr(buf,"IPv6地址:"))
	{
		int i;

		for(i=0;buf[i];++i)
			if(buf[i] == ':')
				break;
		if(buf[i] == '\0')
			return 0;
		for(++i;buf[i] != '<';++i)
			printf("%c",buf[i]);

		printf("\n");
		return 1;
	}

	return 0;
}

void iconv_msg(char *in,int i_len,char *out,int o_len)
{
	iconv_t cd;

	cd=iconv_open("UTF-8//","GBK//");
	iconv(cd,&in,&i_len,&out,&o_len);
	iconv_close(cd);
}

int parse_ipv4(char *buf,char *ip)
{
	if(strstr(buf,ip) && strstr(buf,"IP"))
	{
		int i;
		int index;
		int len;
		char *t;
		char res[512];

		for(i=0;buf[i];++i)
			if(buf[i] == 't' && buf[i+1] == '>')
				break;
		if(buf[i] == '\0')
			return 0;

		index=i+2;
		for(i=index,len=0;buf[i];++i,++len)
			if(buf[i] == '<')
				break;
		if(buf[i] == '\0')
			return 0;
		t=malloc(len);

		for(i=0;len;--len,++index,++i)
			t[i]=buf[index];
		t[i]='\0';

		bzero(res,512);
		iconv_msg(t,strlen(t),res,512);
		printf("%s\n",res);

		free(t);
		return 1;
	}

	return 0;
}

int parse_and_get_ip_addr(char *buf,char *ip,BOOL flage)
{
	if(flage)
		return parse_ipv6(buf);
	else
		return parse_ipv4(buf,ip);
}

int main(int argc,char **argv)
{
	int sockfd;
	SA_IN server_addr;
	char *host;
	char *buf;
	BOOL flage;

	if(argc != 2)
		error_return("query_ip_addr <ip address>");

	flage=is_ipv6(argv[1]);
	if(flage == -1)
		error_return("Invalid ip address");

	if(flage)
		host=H_IPV_6;
	else
		host=H_IPV_4;

	sockfd=Socket(AF_INET,SOCK_STREAM,0);
	init_data_with_client(&server_addr,host,PORT);
	Connect(sockfd,&server_addr);

	send_http_get_request(sockfd,argv[1],flage);

	if((buf=read_line(sockfd)) == NULL)
		error_return("Read Error!");

	if(!strstr(buf,"200"))
		error_return(buf);

	while((buf=read_line(sockfd)))
		if(parse_and_get_ip_addr(buf,argv[1],flage))
			break;

	close(sockfd);
}
