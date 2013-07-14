#include "smbot.h"

#define null_continue() \
{\
	if(temp == NULL)\
	{\
		safe_free(&buf);\
		goto start;\
	}\
}

#define no_arg_continue(msg) \
{\
	if(strstr(temp,msg))\
	{\
		msgto(sockfd,CHANNEL,nick,temp);\
		safe_free(&buf);\
		safe_free(&nick);\
		goto start;\
	}\
}

#define no_result_continue(msg) \
{\
	if(strstr(t,msg))\
	{\
		msgto(sockfd,CHANNEL,nick,t);\
		safe_free(&buf);\
		safe_free(&nick);\
		goto start;\
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
	char *buf=NULL;
	char *temp=NULL;
	char *t=NULL;
	char *nick=NULL;
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
start:
		FD_ZERO(&reads);
		FD_SET(sockfd,&reads);
		ret=select(sockfd+1,&reads,NULL,NULL,NULL);
		if(ret == -1)
			goto start;

		buf=read_line(sockfd);
		if(buf == NULL)
			goto start;
		printf("%s",buf);

		if(strstr(buf,"!man") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!man","!man <target> 或者 !man [0-9] <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!man");
			t=get_man_url(temp);
			safe_free(&temp);
			no_result_continue("Sorry");
			msgto(sockfd,CHANNEL,nick,t);

			safe_free(&t);
			safe_free(&nick);
			goto start;
		}

		if(strstr(buf,"!ip") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!ip","!ip <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!ip");
			t=query_ip(temp);
			safe_free(&temp);

			msgto(sockfd,CHANNEL,nick,t);

			safe_free(&nick);
			safe_free(&t);
			safe_free(&buf);
			goto start;
		}

		if(strstr(buf,"!time") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_time();
			nick=get_nick(buf);
			msgto(sockfd,CHANNEL,nick,temp);

			safe_free(&nick);
			safe_free(&buf);
			goto start;
		}

		if(strstr(buf,"!dict") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!dict","!dict <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!dict");
			t=dict(temp);
			safe_free(&temp);
			no_result_continue("Sorry,no result");
			msgto(sockfd,CHANNEL,nick,t);

			safe_free(&t);
			safe_free(&nick);
			safe_free(&buf);
			goto start;
		}

		if(strstr(buf,"!torrent") && strstr(buf,"PRIVMSG"))
		{
			char *t;

			//printf("%s\n",buf);

			//msgto(sockfd,CHANNEL,get_nick(buf),"该搜索可能会比较慢，请耐心等待，在此期间也希望不要再呼叫我!\n");
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!torrent","!torrent <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!torrent");
			t=torrent(temp);
			safe_free(&temp);
			no_result_continue("Sorry");
			msgto(sockfd,CHANNEL,nick,t);

			safe_free(&t);
			safe_free(&buf);
			safe_free(&nick);
			goto start;
			//msgto(sockfd,CHANNEL,get_nick(buf),"由于无法连接到torrentkitty，所以暂时关闭该功能!\r\n");
		}

		if(strstr(buf,"!youku") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!youku","!youku <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!youku");
			t=get_youku_url(temp);
			safe_free(&temp);
			no_result_continue("Sorry,no result");
			msgto(sockfd,CHANNEL,nick,t);

			safe_free(&t);
			safe_free(&buf);
			safe_free(&nick);
			goto start;
		}

		if(strstr(buf,"!bt") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!bt","!bt <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!bt");
			t=get_bt_magnet(temp);
			safe_free(&temp);
			no_result_continue("Sorry");
			msgto(sockfd,CHANNEL,nick,t);

			safe_free(&t);
			safe_free(&buf);
			safe_free(&nick);
			goto start;
		}

		if(strstr(buf,"!yb") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!yb","!yb <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!yb");
			t=get_yb_code(temp);
			safe_free(&temp);

			msgto(sockfd,CHANNEL,nick,t);

			safe_free(&t);
			safe_free(&buf);
			safe_free(&nick);
			goto start;
		}

		if(strstr(buf,"!weather") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!weather","!weather <target>\n");
			null_continue();
			nick=get_nick(buf);
			no_arg_continue("!weather");
			t=get_weather(temp);
			safe_free(&temp);
			msgto(sockfd,CHANNEL,nick,t);

			safe_free(&t);
			safe_free(&buf);
			safe_free(&nick);
			goto start;
		}

		if(strstr(buf,"!list") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			nick=get_nick(buf);
			msgto(sockfd,CHANNEL,nick,
					"man、ip、time、dict、torrent、youku、bt、yb、weather、list\n");

			safe_free(&nick);
		}

		if(strstr(buf,"PING") && !strstr(buf,"PRIVMSG"))
			pong_ser(sockfd,buf);

		safe_free(&buf);
	}

	close(sockfd);

	return 0;
}

