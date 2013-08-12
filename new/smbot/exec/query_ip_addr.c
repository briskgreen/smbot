#include "../mysock/mysock.h"

typedef int BOOL;

#define TRUE 1
#define FALSE 0
#define H_IPV_4 "ip.chinaz.com"
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
		head=malloc(strlen("GET /?IP=")+strlen(ip)+strlen(" HTTP/1.1\n"));
		strcpy(head,"GET /?IP=");
		host="Host: ip.chinaz.com\n";
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

int parse_ipv4(char *buf,char *ip)
{
	regex_t reg;
	regmatch_t pmatch[1];

	if(strstr(buf,"==>>"))
	{
		char *t;

		if(regcomp(&reg,"==>> [^0-9].*</strong>",0) != 0)
			return 0;
		if(regexec(&reg,buf,1,pmatch,0) != 0)
			return 0;
		regfree(&reg);

		t=malloc(pmatch[0].rm_eo-pmatch[0].rm_so-11);
		bzero(t,pmatch[0].rm_eo-pmatch[0].rm_so-11);
		snprintf(t,pmatch[0].rm_eo-pmatch[0].rm_so-12,"%s",
				buf+pmatch[0].rm_so+4);
		printf("%s\n",t);
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
	{
		if(parse_and_get_ip_addr(buf,argv[1],flage))
			break;
		free(buf);
	}

	if(buf == NULL)
		printf("Sorry,No result found!\n");

	close(sockfd);
}
