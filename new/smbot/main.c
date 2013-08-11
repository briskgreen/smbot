#include "config.h"

bool is_use_ssl=FALSE;

#define parse_arg(reg,des,func,priority) \
{\
	parse_and_perform(task,data,reg,des,func,is_use_ssl,priority);\
	free(data);\
	continue;\
}

void quit(int signum)
{
	if(is_use_ssl)
	{
		SSL_write(ssl,QUIT_MSG,strlen(QUIT_MSG));
		ssl_close(ssl);
	}
	else
	{
		send(sockfd,QUIT_MSG,strlen(QUIT_MSG),0);
		close(sockfd);
	}

	exit(-1);
}

void safe_send_data(SSL *ssl,SMBOT_CONF *conf)
{
	char *nick;
	char *user;
	char *join;

	nick=string_add("NICK %s\n",conf->nick);
	user=string_add("USER %s %s %s :%s\n",conf->nick,
			conf->user_name,conf->server,conf->really_name);

	SSL_write(ssl,nick,strlen(nick));
	SSL_write(ssl,user,strlen(user));
	free(nick);
	free(user);
	
	while(conf->channel->next != NULL)
	{
		conf->channel=conf->channel->next;
		join=string_add("JOIN %s\n",conf->channel->element);
		SSL_write(ssl,join,strlen(join));
		free(join);
	}
}

void send_data(int sockfd,SMBOT_CONF *conf)
{
	char *nick;
	char *user;
	char *join;

	nick=string_add("NICK %s\n",conf->nick);
	user=string_add("USER %s %s %s :%s\n",conf->nick,
			conf->user_name,conf->server,conf->really_name);

	send(sockfd,nick,strlen(nick),0);
	send(sockfd,user,strlen(user),0);
	free(nick);
	free(user);
	
	while(conf->channel->next != NULL)
	{
		conf->channel=conf->channel->next;
		join=string_add("JOIN %s\n",conf->channel->element);
		send(sockfd,join,strlen(join),0);
		free(join);
	}
}

int main(int argc,char **argv)
{
	TASK_FACTORY *task;
	SMBOT_CONF *conf;
	struct sigaction act;
	char *data;

	task=task_factory_init(5,10);
	act.sa_flags=0;
	act.sa_handler=quit;
	sigaction(SIGINT,&act,NULL);

	conf=smbot_conf_init();
	smbot_conf_open(conf);
	smbot_conf_read(conf);
	if(strstr(conf->use_ssl,"true"))
	{
		is_use_ssl=TRUE;
		ssl=ssl_connect(conf->server,conf->port,NULL,NULL);
	}
	else
	{
		is_use_ssl=FALSE;
		sockfd=tcp_connect(conf->server,conf->port);
	}

	if(is_use_ssl)
		safe_send_data(ssl,conf);
	else
		send_data(sockfd,conf);
	smbot_conf_close(conf);

	while(1)
	{
		if(is_use_ssl)
			data=ssl_read_line(ssl);
		else
			data=read_line(sockfd);

		if(strstr(data,"!man"))
			parse_arg("PRIVMSG #[^ ]* :!man","!man [0-9] <要查询的内容> 功能:返回linuxmanpages连接",get_man_url,5);

		if(strstr(data,"!ip"))
		{}

		if(strstr(data,"!time"))
		{}

		if(strstr(data,"!dict"))
		{}

		if(strstr(data,"!dict"))
		{}

		if(strstr(data,"!torrent"))
		{}

		if(strstr(data,"!youku"))
		{}

		if(strstr(data,"!bt"))
		{}

		if(strstr(data,"!zip"))
		{}

		if(strstr(data,"!weather"))
		{}

		if(strstr(data,"!stack"))
		{}

		if(strstr(data,"!id"))
		{}

		if(strstr(data,"!checkid"))
		{}

		if(strstr(data,"!url"))
		{}

		if(strstr(data,"!deurl"))
		{}

		if(strstr(data,"!joke"))
		{}

		if(strstr(data,"!dream"))
		{}

		if(strstr(data,"!song"))
		{}

		if(strstr(data,"!bing"))
		{}

		if(strstr(data,"!google"))
		{}

		if(strstr(data,"!image"))
		{}

		if(strstr(data,"!list"))
		{}

		if(strstr(data,"!smbot"))
		{
			smbot_help(is_use_ssl);
			free(data);
		}

		if(strstr(data,"PING") && !strstr(data,"PRIVMSG"))
			pong_server(data,is_use_ssl);

		printf("%s\n",data);
	}

	return 0;
}
