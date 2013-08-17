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

void get_ip_addr(SMBOT_DATA *data)
{
	pid_t pid;
	int pipefd[2];
	char temp[1024]={0};

	pipe(pipefd);
	if((pid=fork()) == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("exec/query_ip_addr","query_ip_addr",data->arg,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,sizeof(temp));
	waitpid(pid,NULL,WNOHANG);

	msg_send(temp,data);
	smbot_destory(data);
}

void bing_dict(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	HTTP *http;
	int i;

	for(i=0;data->arg[i]!= ' ';++i);
	if(data->arg[i] == '\0')
	{
		msg_send("错误的格式",data);
		smbot_destory(data);
		return;
	}
	buf=strnstr(data->arg,i);

	http=http_head_init();
	res=string_add("GET /Bing/MicrosoftTranslator/v1/Translate?Text=%27%s%27&To=%27%s%27&$skip=0&$top=1&$format=json HTTP/1.1\n",data->arg+i,buf);
	null_no_free(buf);
	http_head_add(http,res);
	null_no_free(res);
	http_head_add(http,"Host: api.datamarket.azure.com\n");
	http_head_add(http,BING_AU);
	http_head_add(http,"Accept: */*\n");
	http_head_add(http,"Connection: close\n\n");

	res=https_perform(http,"api.datamarket.azure.com",443,NULL,NULL);
	if(res == NULL)
	{
		msg_send("Sorry,连接到远程服务器出错",data);
		smbot_destory(data);
		http_head_destroy(http);
		return;
	}

	buf=match_string("\"Text\":.[^\"]*",res);
	null_no_free(res);
	if(buf == NULL)
	{
		msg_send("Sorry no result found!",data);
		smbot_destory(data);
		http_head_destroy(http);
		return;
	}

	res=strnstr(buf+8,strlen(buf)-8);
	null_no_free(buf);
	msg_send(res,data);
	null_no_free(res);
	smbot_destory(data);
	http_head_destroy(http);
}

void get_youke_url(SMBOT_DATA *data)
{
	char *head;
	char *host="Host: www.soku.com\n";
	char *accept="Accept: */*\n";
	char *connection="Connection: close\n\n";
	char *res;
	char *buf;
	int sockfd;
	char *title;
	char *url;

	buf=url_encode(data->arg);
	head=string_add("GET /search_video/q_%s HTTP/1.1\n",buf);
	free(buf);

	sockfd=tcp_connect("www.soku.com",80);
	if(sockfd == -1)
	{
		free(head);
		msg_send("Sorry,连接远程服务器失败!",data);
		smbot_destory(data);
		return;
	}

	send(sockfd,head,strlen(head),0);
	send(sockfd,host,strlen(host),0);
	send(sockfd,accept,strlen(accept),0);
	send(sockfd,connection,strlen(connection),0);
	free(head);

	while(buf=read_line(sockfd))
	{
		if(strstr(buf,"<a title="))
			break;
		free(buf);
	}
	close(sockfd);

	if(buf == NULL)
	{
		msg_send("Sorry,no result found!",datas);
		smbot_destory(data);
		return;
	}

	title=match_string("<a title=\".[^\"]*",buf);
	url=match_string("http://*.[^\"]*",buf);
	free(buf);

	res=string_add("%s <--%s",title+10,url);
	free(title);
	free(url);
	msg_send(res,data);
	smbot_destory(res);
}

void get_bt(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	char *head;
	HTTp *http;

	buf=url_encode(data->arg);
	head=string_add("GET http://btdigg.org/search?info_hash=&q=%s HTTP/1.1\n",buf);
	free(buf);
	http=http_head_init();
	http_head_add(http,head);
	http_head_add(http,"Host: btdigg.org\n");
	http_head_add(http,"Accept: */*\n");
	http_head_add(http,"Connection: close\n\n");

	buf=http_perform(http,"127.0.0.1",8087);
	http_head_destroy(http);
	if(buf == NULL)
	{
		msg_send("Sorry,连接远程服务器失败!",data);
		smbot_destory(data);
		return;
	}
	res=match_string("magnet\:\?xt=urn\:btih\:*.[^\"]*",buf);

	if(res == NULL)
	{
		msg_send("Sorry,no result found!",data);
		smbot_destory(data);
		return;
	}

	msg_send(res,data);
	free(res);
	smbot_destory(data);
}

void get_zip_code(SMBOT_DATA *data)
{
	char *res;
	char code[80]={0};
	char *buf;
	char *temp;

	to_iconv("UTF-8//","GBK//",data->arg,strlen(data->arg),
			code,80);
	buf=string_add("http://opendata.baidu.com/post/s?wd=%s&p=mini&rn=20",code);
	res=http_get_simple(buf,80);
	free(buf);

	buf=strstr(res,"begin");
	if(buf == NULL)
	{
		msg_send("Sorry,no result found!",data);
		free(res);
		smbot_destory(data);
		return;
	}

	temp=match_string("[0-9]\\{6\\}[^<]",buf);
	free(buf);
	if(temp == NULL)
	{
		msg_send("Sorry,no result found!",data);
		free(res);
		smbot_destory(data);
		return;
	}
	
	free(res);
	msg_send(temp,data);
	free(temp);
	smbot_destory(data);
}

void get_weather(SMBOT_DATA *data)
{
	char *temp;
	char *buf;
	char code[1024]={0};

	buf=string_add("php.weather.sina.com.cn/search.php?city=%s&dpc=1",data->arg);
	temp=http_get_simple(buf,80);
	free(buf);
	if(temp == NULL)
	{
		msg_send("Sorry,连接到远程服务器出错!",data);
		smbot_destory(data);
		return;
	}

	buf=match_string("title:\'.[^\']*",temp);
	free(temp);
	if(buf == NULL)
	{
		msg_send("Sorry,no result found!",data);
		smbot_destory(data);
		return;
	}

	to_iconv("GBK//","UTF-8",buf+7,strlen(buf)-7,code,1024);
	free(buf);
	msg_send(code,data);
	smbot_destory(data);
}

void get_stack(SMBOT_DATA *data)
{
	char *url;
	char *des;
	char *res;
	char *buf;
	int i;

	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';
	buf=string_add("http://stackoverflow.com/search?q=%s",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("Sorry,连接到远程服务器出错!",data);
		smbot_destory(data);
		return;
	}

	buf=strstr(res,"Q: ");
	free(res);
	if(buf == NULL)
	{
		msg_send("Sorry,no result found!",data);
		smbot_destory(data);
		return;
	}

	url==match_string("/questions.[^\"]*",buf);
	des=match_string("Q: .[^<]*",buf);
	res=string_add("%s <-- http://stackoverflow.com%s",des,url);
	free(url);
	free(des);
	msg_send(res,data);
	free(res);
	smbot_destory(data);
}

void msg_send(const char *msg,SMBOT_DATA *data)
{
	if(data->is_use_ssl)
		ssl_msgto(ssl,data->channel,data->nick,msg);
	else
		msgto(sockfd,data->channel,data->nick,msg);
}
