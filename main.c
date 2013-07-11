#include "smbot.h"

void quit_irc(int signum)
{
	kill(pid,SIGKILL);

	send(sockfd,"QUIT\r\n",strlen("QUIT\r\n"),0);
	close(sockfd);
	exit(0);
}

int main(int argc,char **argv)
{
	char *buf;
	char *temp;
	char *nick;
	int ret;
	fd_set reads;
	struct sigaction act,old;

	sockfd=tcp_conect(SER,PORT);
	if(sockfd == -1)
		error_quit("Connection");

	act.sa_flags=0;
	act.sa_handler=quit_irc;
	sigaction(SIGINT,&act,&old);

	send(sockfd,NICK,strlen(NICK),0);
	send(sockfd,USER,strlen(USER),0);
	send(sockfd,JOIN,strlen(JOIN),0);

	while(1)
	{
		FD_ZERO(&reads);
		FD_SET(sockfd,&reads);
		ret=select(sockfd+1,&reads,NULL,NULL,NULL);
		if(ret == -1)
			continue;

		buf=read_line(sockfd);
		//printf("%s",buf);

		if(strstr(buf,"!man") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);
			
			temp=get_man_url(buf);
			nick=get_nick(buf);
			msgto(sockfd,CHANNEL,nick,temp);

			if(strstr(temp,"man <target>") || strstr(temp,"参数错误") || strstr(temp,"Sorry"))
			{
				free(buf);
				free(nick);
				continue;
			}

			free(temp);
			free(nick);
		}

		if(strstr(buf,"!ip") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			temp=query_ip(buf);
			nick=get_nick(buf);

			msgto(sockfd,CHANNEL,nick,temp);

			if(strstr(temp,"创建管道") || strstr(temp,"ip <ip add"))
			{
				free(buf);
				free(nick);
				continue;
			}

			free(temp);
			free(nick);
		}

		if(strstr(buf,"!time") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			temp=get_time();
			nick=get_nick(buf);
			msgto(sockfd,CHANNEL,nick,temp);

			free(nick);
		}

		if(strstr(buf,"!dict") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			temp=dict(buf);
			nick=get_nick(buf);
			msgto(sockfd,CHANNEL,nick,temp);

			if(strstr(temp,"dict <wor") || strstr(temp,"Sorry"))
			{
				free(buf);
				free(nick);
				continue;
			}

			free(temp);
			free(nick);
		}

		if(strstr(buf,"!torrent") && strstr(buf,"PRIVMSG"))
		{
			char *t;

			printf("%s\n",buf);

			msgto(sockfd,CHANNEL,get_nick(buf),"该搜索可能会比较慢，请耐心等待，在此期间也希望不要再呼叫我!\r\n");
			temp=torrent(buf);
			nick=get_nick(buf);
			if(strstr(temp,"Sorry") || strstr(temp,"torrent"))
			{
				msgto(sockfd,CHANNEL,nick,temp);

				free(buf);
				free(nick);
				continue;
			}

			t=get_magnet(temp);
			msgto(sockfd,CHANNEL,nick,t);
			if(strstr(t,"Sorry"))
			{
				free(buf);
				free(nick);
				free(temp);
				continue;
			}

			free(nick);
			free(t);
			free(temp);
			//msgto(sockfd,CHANNEL,get_nick(buf),"由于无法连接到torrentkitty，所以暂时关闭该功能!\r\n");
		}

		if(strstr(buf,"!youku") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			nick=get_nick(buf);
			temp=get_youku_url(buf);
			msgto(sockfd,CHANNEL,nick,temp);

			if(strstr(temp,"Sorry") || strstr(buf,"youku"))
			{
				free(buf);
				free(nick);
				continue;
			}

			free(nick);
			free(temp);
		}

		if(strstr(buf,"!bt") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			nick=get_nick(buf);
			temp=get_bt_magnet(buf);
			msgto(sockfd,CHANNEL,nick,temp);

			if(strstr(temp,"bt"))
			{
				free(buf);
				free(nick);
				continue;
			}

			free(temp);
			free(nick);
		}

		if(strstr(buf,"!list") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			nick=get_nick(buf);
			msgto(sockfd,CHANNEL,nick,
					"man、ip、time、dict、torrent、youku、bt、list\r\n");

			free(nick);
		}

		if(strstr(buf,"PING"))
			send(sockfd,buf,sizeof(buf),0);

		free(buf);
		buf=NULL;
	}

	close(sockfd);

	return 0;
}

