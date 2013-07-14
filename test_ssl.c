#include "mysock/mysock.h"
#include <openssl/ssl.h>

#define IRC "irc.freenode.org"
#define PORT 7000
#define NICK "NICK welcombot\n"
#define USER "USER welcombot welcombot irc.freenode.net :welcombot\n"
#define JOIN "JOIN #linukso\n"
#define MSG "PRIVMSG #linukso 基佬！\n"

int main(void)
{
	int sockfd;
	char buf[1024];
	SSL *ssl;
	SSL_CTX *ssl_ctx;
	SSL_METHOD *ssl_method;

	SSL_library_init();
	ssl_method=SSLv3_client_method();
	ssl_ctx=SSL_CTX_new(ssl_method);

	sockfd=tcp_conect(IRC,PORT);
	if(sockfd == -1)
		error_quit("Can not conect!\n");
	ssl=SSL_new(ssl_ctx);
	SSL_set_fd(ssl,sockfd);
	SSL_connect(ssl);

	SSL_write(ssl,NICK,strlen(NICK));
	SSL_write(ssl,USER,strlen(USER));
	SSL_write(ssl,JOIN,strlen(JOIN));

	while(1)
	{
		bzero(buf,sizeof(buf));
		SSL_read(ssl,buf,sizeof(buf)-1);
		printf("%s\n",buf);

		if(strstr(buf,"JOIN") && !strstr(buf,"PRIVMSG"))
			SSL_write(ssl,MSG,strlen(MSG));
	}

	SSL_shutdown(ssl);
	SSL_free(ssl);
	close(sockfd);
	SSL_CTX_free(ssl_ctx);

	return 0;
}
