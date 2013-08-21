#include "smbot.h"

void msgto(int sockfd,const char *channel,const char *nick,
		const char *msg)
{
	int len;
	int n=0;
	char *buf=NULL;
	char *temp;

	len=strlen(msg);

	do
	{
		buf=strnstr(msg+n,400);
		temp=string_add("PRIVMSG %s :%s: %s\n",
				channel,nick,buf+n);

		n=send(sockfd,temp,strlen(temp),0);
		len-=n;
		free(buf);
		free(temp);
	}while(len > 400);
}

void ssl_msgto(SSL *ssl,const char *channel,const char *nick,
		const char *msg)
{
	int len;
	int n=0;
	char *buf=NULL;
	char *temp;

	len=strlen(msg);

	do
	{
		buf=strnstr(msg+n,400);
		temp=string_add("PRIVMSG %s :%s: %s\n",
				channel,nick,buf+n);
		n=SSL_write(ssl,temp,strlen(temp));
		len-=n;
		free(buf);
		free(temp);
	}while(len > 400);
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

char *get_arg(char *buf,char *prg,char *des)
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
		return NULL;
	}
	if(regexec(&preg,buf,1,pmatch,0) != 0)
	{
		regfree(&preg);
		return NULL;
	}
	regfree(&preg);

	index=pmatch[0].rm_eo;
	temp=buf[index];
	if(temp != ' ' && temp == '\r')
		return des;
	else if(temp != ' ' && temp != '\r')
		return NULL;

	for(index+=2,len=1;buf[index];++len,++index);
	res=malloc(len);
	strncpy(res,buf+pmatch[0].rm_eo+1,len-1);
	res[len-1]='\0';

	return res;
}

void pong_server(char *msg,bool is_use_ssl)
{
	msg[1]='O';

	if(is_use_ssl)
		SSL_write(ssl,msg,strlen(msg));
	else
		send(sockfd,msg,strlen(msg),0);

	free(msg);
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
	char *list="man、ip、time、dict、youku、bt、zip、weather、stack、id、checkid、url、deurl、joke、dream、song、bing、google、image、list、baidu、bimg、smbot";
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
	data->arg=get_arg(msg,reg,des);
	data->is_use_ssl=is_use_ssl;
	task_factory_add(task,func,data,priority);
}

void smbot_destory(SMBOT_DATA *data)
{
	null_no_free(data->nick);
	null_no_free(data->channel);
	null_no_free(data->arg);
	safe_free((void **)&data);
}

void null_no_free(char *p)
{
	if(p == NULL)
		return;
	else
		free(p);
}

void time_keeping(CHANNEL *channel,bool is_use_ssl)
{
	struct tm *ti;
	time_t t;
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

	while(1)
	{
		t=time(NULL);
		ti=localtime(&t);

		if(ti->tm_min ==0 && ti->tm_sec == 0)
		{
			if(is_use_ssl)
			{
				char *temp;

				while(channel->next != NULL)
				{
					channel=channel->next;

					temp=string_add("PRIVMSG %s:%s\n",
							channel->element,
							msg[ti->tm_hour % 14]);
					SSL_write(ssl,temp,strlen(temp));
					free(temp);
				}
			}
			else
			{
				char *temp;
				
				while(channel->next != NULL)
				{
					channel=channel->next;

					temp=string_add("PRIVMSG %s:%s\n",
							channel->element,
							msg[ti->tm_hour % 24]);
					send(sockfd,temp,strlen(temp),0);
					free(temp);
				}
			}
		}

		sleep(1);
	}
}
