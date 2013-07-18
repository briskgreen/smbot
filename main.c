#include "smbot.h"

#define null_continue() \
{\
	if(temp == NULL)\
	{\
		safe_free((void **)&buf);\
		goto start;\
	}\
}

#define no_arg_continue(msg) \
{\
	if(strstr(temp,msg))\
	{\
		msgto(sockfd,channel,nick,temp);\
		safe_free((void **)&buf);\
		safe_free((void **)&nick);\
		safe_free((void **)&channel);\
		goto start;\
	}\
}

#define no_result_continue(msg) \
{\
	if(strstr(t,msg))\
	{\
		msgto(sockfd,channel,nick,t);\
		safe_free((void **)&buf);\
		safe_free((void **)&nick);\
		safe_free((void **)&channel);\
		goto start;\
	}\
}

#define safe_free_and_goto() \
{\
	safe_free((void **)&t);\
	safe_free((void **)&buf);\
	safe_free((void **)&nick);\
	safe_free((void **)&channel);\
	goto start;\
}

#define init_and_check(arg) \
{\
	null_continue();\
	nick=get_nick(buf);\
	channel=get_channel(buf);\
	no_arg_continue(arg);\
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
	char *channel=NULL;
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
	send(sockfd,JOIN_DEBIAN_CN,strlen(JOIN_DEBIAN_CN),0);
	send(sockfd,JOIN_LINUXBA,strlen(JOIN_LINUXBA),0);
	send(sockfd,JOIN_LINUXBAR,strlen(JOIN_LINUXBAR),0);
	send(sockfd,JOIN_LINUKSO,strlen(JOIN_LINUKSO),0);

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
			init_and_check("!man");
			t=get_man_url(temp);
			safe_free((void **)&temp);
			no_result_continue("Sorry");
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!ip") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!ip","!ip <target>\n");
			init_and_check("!ip");
			t=query_ip(temp);
			safe_free((void **)&temp);

			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!time") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_time();
			nick=get_nick(buf);
			channel=get_channel(buf);
			msgto(sockfd,channel,nick,temp);

			safe_free((void **)&nick);
			safe_free((void **)&buf);
			safe_free((void **)&channel);
			goto start;
		}

		if(strstr(buf,"!dict") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!dict","!dict <target>\n");
			init_and_check("!dict");
			t=dict(temp);
			safe_free((void **)&temp);
			no_result_continue("Sorry,no result");
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!torrent") && strstr(buf,"PRIVMSG"))
		{
			char *t;

			//printf("%s\n",buf);

			//msgto(sockfd,CHANNEL,get_nick(buf),"该搜索可能会比较慢，请耐心等待，在此期间也希望不要再呼叫我!\n");
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!torrent","!torrent <target>\n");
			init_and_check("!torrent");
			t=torrent(temp);
			safe_free((void **)&temp);
			no_result_continue("Sorry");
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
			//msgto(sockfd,CHANNEL,get_nick(buf),"由于无法连接到torrentkitty，所以暂时关闭该功能!\r\n");
		}

		if(strstr(buf,"!youku") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!youku","!youku <target>\n");
			init_and_check("!youku");
			t=get_youku_url(temp);
			safe_free((void **)&temp);
			no_result_continue("Sorry,no result");
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!bt") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!bt","!bt <target>\n");
			init_and_check("!bt");
			t=get_bt_magnet(temp);
			safe_free((void **)&temp);
			no_result_continue("Sorry");
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!yb") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!yb","!yb <target>\n");
			init_and_check("!yb");
			t=get_yb_code(temp);
			safe_free((void **)&temp);

			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!weather") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!weather","!weather <target>\n");
			init_and_check("!weather");
			t=get_weather(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!stack") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!stack","!stack <questions>\n");
			init_and_check("!stack");
			t=get_stack_over(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!id") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!id","!id <id>\n");
			init_and_check("!id");
			t=get_id_info(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!checkid") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!checkid","!checkid <id>\n");
			init_and_check("!checkid");
			t=check_id_card(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!url") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!url","!url <url>\n");
			init_and_check("!url");
			t=get_url_encode(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!deurl") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!deurl","!deurl <code>\n");
			init_and_check("!deurl");
			t=get_url_decode(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!joke") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!joke","!joke <word>\n");
			init_and_check("!joke");
			t=get_joke(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!dream") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!dream","!dream <dream>\n");
			init_and_check("!dream");
			t=get_dream(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!song") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!song","!song <song>\n");
			init_and_check("!song");
			t=get_song_url(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!bing") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!bing","!bing <word>\n");
			init_and_check("!bing");
			t=get_bing_result(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!google") && strstr(buf,"PRIVMSG"))
		{
			temp=get_arg(buf,"PRIVMSG #.[^ ]* :!google","!google <word>\n");
			init_and_check("!google");
			t=get_google_result(temp);
			safe_free((void **)&temp);
			msgto(sockfd,channel,nick,t);

			safe_free_and_goto();
		}

		if(strstr(buf,"!list") && strstr(buf,"PRIVMSG"))
		{
			//printf("%s\n",buf);

			nick=get_nick(buf);
			channel=get_channel(buf);
			msgto(sockfd,channel,nick,
					"man、ip、time、dict、torrent、youku、bt、yb、weather、stack、id、checkid、url、deurl、joke、dream、song、bing、google、list\n");

			safe_free((void **)&nick);
			safe_free((void **)&channel);
		}

		if(strstr(buf,"PING") && !strstr(buf,"PRIVMSG"))
			pong_ser(sockfd,buf);

		safe_free((void **)&buf);
	}

	close(sockfd);

	return 0;
}

