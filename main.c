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
	char *identify;
	char *join;
	char *buf;

	nick=string_add("NICK %s\n",conf->nick);
	user=string_add("USER %s %s %s :%s\n",conf->nick,
			conf->user_name,conf->server,conf->really_name);
	identify=string_add("PRIVMSG NickServ :IDENTIFY %s\n",conf->passwd);

	SSL_write(ssl,nick,strlen(nick));
	SSL_write(ssl,user,strlen(user));
	SSL_write(ssl,identify,strlen(identify));
	free(nick);
	free(user);
	free(identify);

	while(buf=ssl_read_line(ssl))
	{
		printf("%s\n",buf);

		if(strstr(buf,"You are now identified for"))
		{
			free(buf);
			break;
		}

		if(strstr(buf,"Invalid password for"))
		{
			free(buf);
			printf("Invalid password!\n");
			exit(-1);
		}

		free(buf);
	}

	if(buf == NULL)
		exit(-1);
	
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
	char *identify;
	char *join;
	char *buf;

	nick=string_add("NICK %s\n",conf->nick);
	user=string_add("USER %s %s %s :%s\n",conf->nick,
			conf->user_name,conf->server,conf->really_name);
	identify=string_add("PRIVMSG NickServ :IDENTIFY %s\n",conf->passwd);

	send(sockfd,nick,strlen(nick),0);
	send(sockfd,user,strlen(user),0);
	send(sockfd,identify,strlen(identify),0);
	free(nick);
	free(user);
	free(identify);

	while(buf=read_line(sockfd))
	{
		printf("%s\n",buf);

		if(strstr(buf,"You are now identified for"))
		{
			free(buf);
			break;
		}

		if(strstr(buf,"Invalid password for"))
		{
			free(buf);
			printf("Invalid password!\n");
			exit(-1);
		}

		free(buf);
	}

	if(buf == NULL)
		exit(-1);
	
	while(conf->channel->next != NULL)
	{
		conf->channel=conf->channel->next;
		join=string_add("JOIN %s\n",conf->channel->element);
		send(sockfd,join,strlen(join),0);
		free(join);
	}
}

void send_time(bool is_use_ssl,char *data)
{
	time_t t;
	char *ti;
	char *res;
	char *nick;
	char *channel;

	nick=get_nick(data);
	channel=get_channel(data);
	t=time(NULL);
	ti=ctime(&t);
	res=string_add("PRIVMSG %s :%s: %s",channel,nick,ti);
	free(nick);
	free(channel);

	if(is_use_ssl)
		SSL_write(ssl,res,strlen(res));
	else
		send(sockfd,ti,strlen(res),0);

	free(res);
}

int main(int argc,char **argv)
{
	TASK_FACTORY *task;
	SMBOT_CONF *conf;
	struct sigaction act;
	char *data;

	printf("init task factory . . .\n");
	task=task_factory_init(20,100);
	printf("init task factory successed . . .\n");
	printf("set signal . . .\n");
	act.sa_flags=0;
	act.sa_handler=quit;
	sigaction(SIGINT,&act,NULL);
	printf("set signal successed . . .\n");

	printf("read config . . .\n");
	conf=smbot_conf_init();
	smbot_conf_open(conf);
	smbot_conf_read(conf);
	printf("read config successed . . .\n");
	printf("connect irc . . .\n");
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
	printf("connect irc successed . . .\n");

	if(is_use_ssl)
		safe_send_data(ssl,conf);
	else
		send_data(sockfd,conf);

	task_factory_add(task,time_keeping,&is_use_ssl,1);
	smbot_conf_close(conf);

	while(1)
	{
		if(is_use_ssl)
			data=ssl_read_line(ssl);
		else
			data=read_line(sockfd);

		printf("%s\n",data);

		if(strstr(data,"!man"))
			parse_arg("PRIVMSG #[^ ]* :!man","!man [0-9] <要查询的内容> 功能:返回linuxmanpages连接",get_man_url,5);

		if(strstr(data,"!ip"))
			parse_arg("PRIVMSG #[^ ]* :!ip","!ip <ipv4/6地址或域名> 功能:返回ipv4/6或者域名的物理位置",get_ip_addr,5);

		if(strstr(data,"!time"))
		{
			send_time(is_use_ssl,data);
			free(data);
			continue;
		}

		if(strstr(data,"!dict"))
			parse_arg("PRIVMSG #[^ ]* :!dict","!dict <目标语言> <要查询的内容> 功能:bing翻译",bing_dict,3);

		if(strstr(data,"!youku"))
			parse_arg("PRIVMSG #[^ ]* :!youku","!youku <关键词> 功能:返回youku第一个链接",get_youku_url,4);

		if(strstr(data,"!bt"))
			parse_arg("PRIVMSG #[^ ]* :!bt","!bt <关键词> 功能:返回btdigg.org第一个磁力链接",get_bt,4);

		if(strstr(data,"!zip"))
			parse_arg("PRIVMSG #[^ ]* :!zip","!zip <地名> 功能:查询邮政编码",get_zip_code,4);

		if(strstr(data,"!weather"))
			parse_arg("PRIVMSG #[^ ]* :!weather","!weather <地名> 功能:天气预报查询",get_weather,3);

		if(strstr(data,"!stack"))
			parse_arg("PRIVMSG #[^ ]* :!stack","!stack <问题> 功能:返回stackoverflow第一个搜索",get_stack,3);

		if(strstr(data,"!id"))
			parse_arg("PRIVMSG #[^ ]* :!id","!id <身份证号码> 功能:身份证号码查询",get_id_information,5);

		if(strstr(data,"!checkid"))
			parse_arg("PRIVMSG #[^ ]* :!checkid","!checkid <身份证号码> 功能:校验身份证最后一位",check_id_card,5);

		if(strstr(data,"!url"))
			parse_arg("PRIVMSG #[^ ]* :!url","!url <信息> 功能:url编码",get_url_encode,5);

		if(strstr(data,"!deurl"))
			parse_arg("PRIVMSG #[^ ]* :!deurl","!deurl <url编码> 功能:url解码",get_url_decode,5);

		if(strstr(data,"!joke"))
			parse_arg("PRIVMSG #[^ ]* :!joke","!joke <关键词> 功能:返回一个小笑话",get_joke,2);

		if(strstr(data,"!dream"))
			parse_arg("PRIVMSG #[^ ]* :!dream","!dream <关键词> 功能:周公解梦",get_dream,2);

		if(strstr(data,"!song"))
			parse_arg("PRIVMSG #[^ ]* :!song","!song <歌曲> 功能:返回百度音乐第一个搜索结果",get_song_url,2);

		if(strstr(data,"!bing"))
			parse_arg("PRIVMSG #[^ ]* :!bing","!bing <关键词> 功能:返回bing搜索第一个结果",get_bing,1);

		if(strstr(data,"!image"))
			parse_arg("PRIVMSG #[^ ]* :!image","!image <关键词> 功能:返回google搜索第一张图片链接",get_google_image_url,1);

		if(strstr(data,"!google"))
			parse_arg("PRIVMSG #[^ ]* :!google","!google <关键词> 功能:返回google搜索第一个结果",get_google,1);

		if(strstr(data,"!baidu"))
			parse_arg("PRIVMSG #[^ ]* :!baidu","!baidu <关键词> 功能:返回baidu搜索第一个结果",get_baidu,1);

		if(strstr(data,"!bimg"))
			parse_arg("PRIVMSG #[^ ]* :!bimg","!bimg <关键词> 功能:返回百度图片搜索第一个结果",get_bimg,1);

		if(strstr(data,"!air"))
			parse_arg("PRIVMSG #[^ ]* :!air","!air <城市名称> 功能:查询城市空气质量，城市名称使用拼音",get_air,1);

		if(strstr(data,"!website"))
			parse_arg("PRIVMSG #[^ ]* :!website","!website <域名> 功能:网站安全检测，检测结果由360提供",get_website_testing,1);

		if(strstr(data,"!wifi"))
			parse_arg("PRIVMSG #[^ ]* :!wifi","!wifi <城市名称> 功能:根据城市名返回查到的第一个wifi热点",get_wifi,1);

		if(strstr(data,"!train"))
			parse_arg("PRIVMSG #[^ ]* :!train","!train <车次> 功能:火车时刻表查询",get_train,1);

		if(strstr(data,"!sm"))
			parse_arg("PRIVMSG #[^ ]* :!sm","!sm <你想要说的话> 功能:与机器人对话，使用\'问题%答案\'方式可调教机器人",get_sm_message,1);

		if(strstr(data,"!list"))
		{
			smbot_list(data,is_use_ssl);
			free(data);
			continue;
		}

		if(strstr(data,"!help smbot"))
		{
			smbot_help(data,is_use_ssl);
			free(data);
			continue;
		}

		if(strstr(data,"PING") && !strstr(data,"PRIVMSG"))
			pong_server(data,is_use_ssl);
	}

	return 0;
}
