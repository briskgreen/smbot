#include "smbot.h"

#define null_continue() \
{\
	if(temp == NULL)\
	{\
		free(buf);\
		continue;\
	}\
}

#define no_arg_continue(msg) \
{\
	if(strstr(temp,msg))\
	{\
		msgto(sockfd,CHANNEL,nick,temp);\
		free(buf);\
		free(nick);\
		continue;\
	}\
}

#define no_result_continue(msg) \
{\
	if(strstr(t,msg))\
	{\
		msgto(sockfd,CHANNEL,nick,t);\
		free(buf);\
		free(nick);\
		continue;\
	}\
}

void quit_irc(int signum)
{
	//kill(pid,SIGKILL);

	send(sockfd,"QUIT\n",strlen("QUIT\n"),0);
	close(sockfd);
	exit(0);
}

int main(int argc,char **argv)
{
	char *buf;
	char *temp;
	char *t;
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
		printf("%s",buf);

		if(strstr(buf,"!man") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);
			
			temp=get_arg(buf,"\\!man","!man <target> 或者 !man [0-9] <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!man");
			t=get_man_url(temp);
			free(temp);
			no_result_continue("Sorry");
			msgto(sockfd,CHANNEL,nick,t);

			free(t);
			free(nick);
			continue;
		}

		if(strstr(buf,"!ip") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			temp=get_arg(buf,"\\!ip","!ip <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!ip");
			t=query_ip(temp);
			free(temp);

			msgto(sockfd,CHANNEL,nick,t);

			free(t);
			free(buf);
			free(nick);
			continue;
		}

		if(strstr(buf,"!time") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			temp=get_time();
			nick=get_nick(buf);
			msgto(sockfd,CHANNEL,nick,temp);

			free(nick);
			free(buf);
			continue;
		}

		if(strstr(buf,"!dict") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			temp=get_arg(buf,"\\!dict","!dict <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!dict");
			t=dict(temp);
			free(temp);
			no_result_continue("Sorry,no result");
			msgto(sockfd,CHANNEL,nick,t);

			free(t);
			free(nick);
			free(buf);
			continue;
		}

		if(strstr(buf,"!torrent") && strstr(buf,"PRIVMSG"))
		{
			char *t;

			printf("%s\n",buf);

			msgto(sockfd,CHANNEL,get_nick(buf),"该搜索可能会比较慢，请耐心等待，在此期间也希望不要再呼叫我!\n");
			temp=get_arg(buf,"\\!torrent","!torrent <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!torrent");
			t=torrent(temp);
			free(temp);
			no_result_continue("Sorry");
			msgto(sockfd,CHANNEL,nick,t);

			free(t);
			free(buf);
			free(nick);
			continue;
			//msgto(sockfd,CHANNEL,get_nick(buf),"由于无法连接到torrentkitty，所以暂时关闭该功能!\r\n");
		}

		if(strstr(buf,"!youku") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			temp=get_arg(buf,"\\!youku","!youku <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!youku");
			t=get_youku_url(temp);
			free(temp);
			no_result_continue("Sorry,no result");
			msgto(sockfd,CHANNEL,nick,t);

			free(t);
			free(buf);
			free(nick);
			continue;
		}

		if(strstr(buf,"!bt") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			temp=get_arg(buf,"\\!bt","!bt <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!bt");
			t=get_bt_magnet(temp);
			free(temp);
			no_result_continue("Sorry");
			msgto(sockfd,CHANNEL,nick,t);

			free(t);
			free(buf);
			free(nick);
			continue;
		}

		if(strstr(buf,"!list") && strstr(buf,"PRIVMSG"))
		{
			printf("%s\n",buf);

			nick=get_nick(buf);
			msgto(sockfd,CHANNEL,nick,
					"man、ip、time、dict、torrent、youku、bt、list\n");

			free(nick);
		}

		if(strstr(buf,"PING"))
			pong_ser(sockfd,buf);

		free(buf);
		buf=NULL;
	}

	close(sockfd);

	return 0;
}

