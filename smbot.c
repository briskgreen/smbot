#include "smbot.h"

void get_channel_list(CHANNEL *channel);
void time_send_ssl(SSL *ssl,CHANNEL *channel,char *msg);
void time_send_sock(int sockfd,CHANNEL *channel,char *msg);

void msgto(int sockfd,const char *channel,const char *nick,
		char *msg)
{
	task_factory_entry();

	int len;
	int n=350;
	int index=0;
	//int post_len;
	int send_n=0;
	char *buf=NULL;
	char *temp;
	int count=0;

	for(len=0;msg[len];++len)
		if(msg[len] == '\n' || msg[len] == '\r')
			msg[len]=' ';

	len=strlen(msg);
	send_n=len;

	if(len > 350)
	{
		while(len > 0)
		{
			index=n;
			for(;msg[n] && n < send_n;++n)
			{
				if((msg[n]&0xe0) == 0xe0)
					break;
				else if((msg[n]&0xc0) == 0xc0)
					break;
				else if((msg[n]&0x80) == 0)
					break;
			}

			temp=strnstr(msg+index-350,350+n-index);
			if(channel == NULL)
				buf=string_add("PRIVMSG %s :%s\n",nick,temp);
			else
				buf=string_add("PRIVMSG %s :%s: %s\n",channel,nick,temp);

			send(sockfd,buf,strlen(buf),0);
			free(buf);
			free(temp);
			len-=(350+n-index);
			n+=350;

			++count;
			if(count > 5)
				sleep(1);
			else
				usleep(80000);
		}
	}
	else
	{
		if(channel == NULL)
			buf=string_add("PRIVMSG %s :%s\n",nick,msg);
		else
			buf=string_add("PRIVMSG %s :%s: %s\n",
					channel,nick,msg);

		send(sockfd,buf,strlen(buf),0);
		free(buf);
	}

/*	if(channel == NULL)
		post_len=strlen("PRIVMSG ")+strlen(nick)+3;
	else
		post_len=strlen("PRIVMSG ")+strlen(channel)+strlen(nick)+5;

	len=strlen(msg);
	n=420-post_len;
	n/=3;
	n*=3;

	while(len >= 0)
	{
		buf=strnstr(msg+send_n,n);
		if(channel == NULL)
			temp=string_add("PRIVMSG %s :%s\n",nick,buf);
		else
			temp=string_add("PRIVMSG %s :%s: %s\n",
					channel,nick,buf);

		send(sockfd,temp,strlen(temp),0);
		len-=n;
		send_n+=n;
		free(buf);
		free(temp);
	}*/

	task_factory_leave();
}

void ssl_msgto(SSL *ssl,const char *channel,const char *nick,
		char *msg)
{
	task_factory_entry();

	int len;
	int n=350;
	int index=0;
	int send_n=0;
	//int post_len=0;
	char *buf=NULL;
	char *temp;
	int count=0;

	for(len=0;msg[len];++len)
		if(msg[len] == '\n' || msg[len] == '\r')
			msg[len]=' ';

	len=strlen(msg);
	send_n=len;

	if(len > 350)
	{
		while(len > 0)
		{
			index=n;
			for(;msg[n] && n < send_n;++n)
			{
				if((msg[n]&0xe0) == 0xe0)
					break;
				else if((msg[n]&0xc0) == 0xc0)
					break;
				else if((msg[n]&0x80) == 0)
					break;
			}

			temp=strnstr(msg+index-350,350+n-index);
			if(channel == NULL)
				buf=string_add("PRIVMSG %s :%s\n",nick,temp);
			else
				buf=string_add("PRIVMSG %s :%s: %s\n",channel,nick,temp);

			SSL_write(ssl,buf,strlen(buf));
			free(buf);
			free(temp);
			len-=(350+n-index);
			n+=350;
			
			++count;
			if(count > 5)
				sleep(1);
			else
				usleep(80000);
		}
	}
	else
	{
		if(channel == NULL)
			buf=string_add("PRIVMSG %s :%s\n",nick,msg);
		else
			buf=string_add("PRIVMSG %s :%s: %s\n",
					channel,nick,msg);

		SSL_write(ssl,buf,strlen(buf));
		free(buf);
	}
	/*if(channel == NULL)
		post_len=strlen("PRIVMSG ")+strlen(nick)+3;
	else
		post_len=strlen("PRIVMSG ")+strlen(channel)+strlen(nick)+5;

	len=strlen(msg);
	n=420-post_len;
	n/=3;
	n*=3;

	while(len >= 0)
	{
		buf=strnstr(msg+send_n,n);
		if(channel == NULL)
			temp=string_add("PRIVMSG %s :%s\n",nick,buf);
		else
			temp=string_add("PRIVMSG %s :%s: %s\n",
					channel,nick,buf);
		SSL_write(ssl,temp,strlen(temp));
		len-=n;
		send_n+=n;
		free(buf);
		free(temp);
	}*/

	task_factory_leave();
}

char *get_nick(char *msg)
{
	char *buf=NULL;
	int len;

	for(len=1;msg[len] != '!';++len);
	buf=malloc(len);
	strncpy(buf,msg+1,len-1);
	buf[len-1]='\0';

	return buf;
}

char *get_arg(char *buf,char *prg,char *des,bool *have_arg)
{
	regex_t preg;
	regmatch_t pmatch[1];
	char *res;
	int index;
	int len;
	char temp;

	if(regcomp(&preg,prg,0) != 0)
	{
		regfree(&preg);
		*have_arg=FALSE;
		return NULL;
	}
	if(regexec(&preg,buf,1,pmatch,0) != 0)
	{
		regfree(&preg);
		*have_arg=FALSE;
		return NULL;
	}
	regfree(&preg);

	index=pmatch[0].rm_eo;
	temp=buf[index];
	if(temp != ' ' && temp == '\r')
	{
		*have_arg=FALSE;
		return des;
	}
	else if(temp != ' ' && temp != '\r')
	{
		*have_arg=FALSE;
		return NULL;
	}

	*have_arg=TRUE;
	for(index+=2,len=1;buf[index];++len,++index);
	res=malloc(len);
	strncpy(res,buf+pmatch[0].rm_eo+1,len-1);
	res[len-1]='\0';

	return res;
}

int pong(char *reg,char *data)
{
	regex_t preg;
	regmatch_t pmatch[1];

	if(regcomp(&preg,reg,0) != 0)
	{
		regfree(&preg);
		return 0;
	}

	if(regexec(&preg,data,1,pmatch,0) != 0)
	{
		regfree(&preg);
		return 0;
	}

	regfree(&preg);
	return 1;
}

void pong_server(char *msg,bool is_use_ssl)
{
	msg[1]='O';

	if(is_use_ssl)
	{
		SSL_write(ssl,msg,strlen(msg));
		SSL_write(ssl,"\n",1);
	}
	else
	{
		send(sockfd,msg,strlen(msg),0);
		send(sockfd,"\n",1,0);
	}

	free(msg);
}

void notice(char *data,bool is_use_ssl)
{
	char *nick=get_nick(data);
	char *ping=strstr(data,"smbot :")+7;
	char *msg;

	msg=malloc(strlen(nick)+strlen(ping)+strlen("NOTICE ")+4);
	sprintf(msg,"NOTICE %s :%s\n",nick,ping);

	if(is_use_ssl)
		SSL_write(ssl,msg,strlen(msg));
	else
		send(sockfd,msg,strlen(msg),0);

	free(msg);
	free(nick);
}

char *get_channel(char *msg)
{
	char *buf;
	regex_t reg;
	regmatch_t pmatch[1];
	int len;

	if(regcomp(&reg,"#.[^ ]*",0) != 0)
	{
		regfree(&reg);
		return NULL;
	}
	if(regexec(&reg,msg,1,pmatch,0) != 0)
	{
		regfree(&reg);
		return NULL;
	}
	regfree(&reg);

	len=pmatch[0].rm_eo-pmatch[0].rm_so+1;
	buf=malloc(len);
	strncpy(buf,msg+pmatch[0].rm_so,len-1);
	buf[len-1]='\0';

	return buf;
}

void smbot_list(const char *msg,bool is_use_ssl)
{
	char *list="man、ip、time、dict、youku、yt、bt、zip、weather、stack、id、checkid、url、deurl、joke、dream、song、bing、google、image、list、baidu、bimg、news、air、website、wifi、train、sm、help smbot";
	char *nick;
	char *channel;

	nick=get_nick(msg);
	channel=get_channel(msg);
	if(is_use_ssl)
		ssl_msgto(ssl,channel,nick,list);
	else
		msgto(sockfd,channel,nick,list);

	free(channel);
	free(nick);
}

void smbot_help(const char *msg,bool is_use_ssl)
{
	char *help="smbot使用!+指令+参数,通过!list指令获取smbot的所有指令。任何一个指令在其指令后不跟参数的情况下均会给出该指令所对应的功能";
	char *channel;
	char *nick;

	channel=get_channel(msg);
	nick=get_nick(msg);
	if(is_use_ssl)
		ssl_msgto(ssl,channel,nick,help);
	else
		msgto(sockfd,channel,nick,help);

	free(channel);
	free(nick);
}

void parse_and_perform(TASK_FACTORY *task,char *msg,char *reg,
		char *des,task_callback func,bool is_use_ssl,
		unsigned int priority)
{
	SMBOT_DATA *data;

	data=malloc(sizeof(SMBOT_DATA));
	data->nick=get_nick(msg);
	data->channel=get_channel(msg);
	data->arg=get_arg(msg,reg,des,&data->have_arg);
	data->is_use_ssl=is_use_ssl;
	task_factory_add(task,func,data,priority);
}

void smbot_destory(SMBOT_DATA *data)
{
	null_no_free(data->nick);
	null_no_free(data->channel);
	/*if(!strstr(data->arg,arg))
		null_no_free(data->arg);*/
	safe_free((void **)&data);
}

void null_no_free(char *p)
{
	if(p == NULL)
		return;
	else
		free(p);
}

void time_keeping(bool *is_use_ssl)
{
	struct tm *ti;
	time_t t;
	CHANNEL *channel;
	char *msg[]={"\1ACTION 0点了，再不碎觉，饥佬们小心不生精啊，腐女们小心会平胸哦!\1",
	"\1ACTION 1点了，还没有睡？正是发福利的好时间!\1",
	"\1ACTION 2点了，这个点还没睡觉，注定孤独一生!\1",
	"\1ACTION 3点了，3点了，碎觉碎觉!\1",
	"\1ACTION 4点了，你是起床了还是还没睡呢？注意身体哦!\1",
	"\1ACTION 5点了，5点了，早起的虫子有鸟吃!\1",
	"\1ACTION 6点了，呦，呦，切克闹，煎饼果子来一套!\1",
	"\1ACTION 7点了，正是来一发的好钟点!\1",
	"\1ACTION 8点了，都8点了，我和我的小伙伴们都惊呆了!\1",
	"\1ACTION 9点了，时间过的好快，快的我都快睡着了!\1",
	"\1ACTION 10点了，00后表示，什么时候才放学啊!\1",
	"\1ACTION 11点了，这个点是该想想中午该吃点啥了!\1",
	"\1ACTION 12点了，酒足饭饱思淫欲，正是调戏妹纸的好时间!\1",
	"\1ACTION 13点了，这个点不午睡都对不起普天百姓，碎觉!\1",
	"\1ACTION 14点了，哎呀，还想再睡一会，哎，可是一个人睡太无聊了!\1",
	"\1ACTION 15点了，请妹纸们喝茶了饥佬们!\1",
	"\1ACTION 16点了，晚饭吃点什么呢？\1",
	"\1ACTION 17点了，16点你丫就开始想着吃晚饭了，吃货注孤生!\1",
	"\1ACTION 18点了，现在是该想想吃点什么了!\1",
	"\1ACTION 19点了，我是该看电视剧还是上irc?\1",
	"\1ACTION 20点了，又是一天，反反复复，一人吃饱，全家不饿!\1",
	"\1ACTION 21点了，贴吧水两贴去吧!\1",
	"\1ACTION 22点了，来点福利吧!\1",
	"\1ACTION 23点了，往事历历在目，让我终于醒悟，光棍还是很有前途，跟着狐朋狗友大家抽烟喝酒，一玩一宿，不用发愁!\1",NULL};

	channel=http_head_init();
	get_channel_list(channel);

	while(1)
	{
		t=time(NULL);
		ti=localtime(&t);

		if(ti->tm_min ==0 && ti->tm_sec == 0)
		{
			if(*is_use_ssl)
				time_send_ssl(ssl,channel,msg[ti->tm_hour % 24]);
			else
				time_send_sock(sockfd,channel,msg[ti->tm_hour % 24]);
		}

		sleep(1);
	}
}

void get_channel_list(CHANNEL *channel)
{
	FILE *fp;
	char buf[1024]={0};
	char *res;
	int len;
	int n=0;
	char temp[512]={0};

	if((fp=fopen(".config","rb")) == NULL)
		_exit(-1);
	fseek(fp,0L,SEEK_END);
	len=ftell(fp);
	rewind(fp);
	fread(buf,len,1,fp);
	fclose(fp);

	res=strstr(buf,"channel #");

	for(len=8;res[len];++len)
	{
		temp[n]=res[len];

		if(res[len] == ' ' || res[len] == '\n')
		{
			char *t;

			t=strnstr(temp,n);
			http_head_add(channel,t);
			n=0;
			bzero(temp,sizeof(temp));
			continue;
		}

		++n;
	}
}

void time_send_ssl(SSL *ssl,CHANNEL *channel,char *msg)
{
	task_factory_entry();

	char *res;

	while(channel->next != NULL)
	{
		channel=channel->next;

		res=string_add("PRIVMSG %s :%s\n",channel->element,msg);
		SSL_write(ssl,res,strlen(res));
		free(res);
	}

	task_factory_leave();
}

void time_send_sock(int sockfd,CHANNEL *channel,char *msg)
{
	task_factory_entry();

	char *res;

	while(channel->next != NULL)
	{
		channel=channel->next;

		res=string_add("PRIVMSG %s :%s\n",channel->element,msg);
		send(sockfd,res,strlen(res),0);
		free(res);
	}

	task_factory_leave();
}
