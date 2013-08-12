#include "command.h"

void msg_send(const char *msg,SMBOT_DATA *data);

void get_man_url(SMBOT_DATA *data)
{
	char *buf;
	char *res;

	if((data->arg[0] <= '9' && data->arg[0] >= '1') && data->arg[1] == ' ')
		buf=string_add("term=%s&section=%c&submitted=1\n\n",
				data->arg+2,data->arg[0]);
	else if((data->arg[0] > '9' || data->arg[0] < '0') && data->arg[1] != ' ')
		buf=string_add("term=%s&section=-1&submitted=1\n\n",
				data->arg);

	res=http_post_simple("http://www.linuxmanpages.com/search.php",
			80,buf);
	null_no_free(buf);
	if(res == NULL)
	{
		msg_send("Sorry,连接到远程服务器出错!",data);
		smbot_destory(data);

		return;
	}

	buf=match_string("location: .[^\r]*",res);
	if(buf == NULL)
	{
		msg_send("Sorry,no result found!",data);
		smbot_destory(data);
		free(res);

		return;
	}

	free(res);
	res=string_add("http://www.linuxmanpages.com%s",buf+10);
	free(buf);

	msg_send(res,data);
	null_no_free(res);
	smbot_destory(data);
}

void msg_send(const char *msg,SMBOT_DATA *data)
{
	if(data->is_use_ssl)
		ssl_msgto(ssl,data->channel,data->nick,msg);
	else
		msgto(sockfd,data->channel,data->nick,msg);
}
