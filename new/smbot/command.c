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

void get_youku_url(SMBOT_DATA *data)
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
		msg_send("Sorry,no result found!",data);
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
	HTTP *http;

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
	res=match_string("magnet:\?xt=urn:btih:*.[^\"]*",buf);

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
	char *res;
	char *buf;

	buf=string_add("http://v.juhe.cn/weather/index?cityname=%s&key=6a046fb4af1c75fa2025d6887f10b113",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	if(strstr(res,"resultcode\":\"202\""))
	{
		free(res);
		msg_send("查询不到该城市的信息",data);
		smbot_destory(data);
		return;
	}

	buf=match_string("temperature.*uv_index",res);
	free(res);
	res=strnstr(buf,-11);
	free(buf);
	msg_send(res,data);
	free(res);
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

	url=match_string("/questions.[^\"]*",buf);
	des=match_string("Q: .[^<]*",buf);
	res=string_add("%s <-- http://stackoverflow.com%s",des,url);
	free(url);
	free(des);
	msg_send(res,data);
	free(res);
	smbot_destory(data);
}

void get_id_information(SMBOT_DATA *data)
{
	char *res;
	char *buf;

	buf=string_add("http://apis.juhe.cn/idcard/index?key=94d452914f35a3cf94b6e28c36c76c77&cardno=%s",data->arg);
	res=http_get_simple(buf,80);
	free(buf);

	buf=match_string("area.[^{]*",res);
	free(res);
	msg_send(buf,data);
	free(buf);
	smbot_destory(data);
}

void check_id_card(SMBOT_DATA *data)
{
	char temp[1024]={0};
	pid_t pid;
	int pipefd[2];

	pipe(pipefd);
	
	if((pid=fork()) == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);
		execl("exec/id_card_test","id_card_test",data->arg,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,sizeof(temp));
	waitpid(pid,NULL,WNOHANG);

	msg_send(temp,data);
	smbot_destory(data);
}

void get_url_encode(SMBOT_DATA *data)
{
	char *res;

	res=url_encode(data->arg);
	msg_send(res,data);
	free(res);
	smbot_destory(data);
}

void get_url_decode(SMBOT_DATA *data)
{
	char *res;

	res=url_decode(data->arg);
	msg_send(res,data);
	free(res);
	smbot_destory(data);
}

void get_joke(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	char temp[512];
	int i;

	buf=string_add("http://www.fangtang8.com/?s=%s",data->arg);
	res=http_get_simple(buf,80);
	free(buf);

	buf=match_string("<p>.[^\\s]*</p>",res);
	free(res);
	strreplace(buf,"<p>","",temp,511);
	free(buf);
	res=string_add("%s",temp);
	bzero(temp,512);
	strreplace(res,"</p>","",temp,511);
	free(res);

	for(i=0;temp[i];++i)
		if(temp[i] == '\n')
			temp[i]=' ';
	msg_send(temp,data);
	smbot_destory(data);
}

void get_dream(SMBOT_DATA *data)
{
	int pipefd[2];
	char temp[512]={0};
	int i;
	pid_t pid;

	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';
	pipe(pipefd);

	if((pid=fork()) == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);
		execl("exec/dream.sh","dream",data->arg,NULL);
	}

	close(pipefd[1]);
	read(pipefd[0],temp,512);
	waitpid(pid,NULL,WNOHANG);

	msg_send(temp,data);
	smbot_destory(data);
}

void get_song_url(SMBOT_DATA *data)
{
	char *buf;
	char *res;
	char url[512];

	buf=string_add("http://music.baidu.com/search?key=%s",data->arg);
	res=http_get_simple(buf,80);
	free(buf);

	buf=match_string("<a href=\"/song/.[^\"]*",res);
	free(res);
	strreplace(buf,"<a href=\"","http://music.baidu.com",url,512);
	free(buf);
	
	msg_send(url,data);
	smbot_destory(data);
}

void get_bing(SMBOT_DATA *data)
{
	char *url;
	char *buf;
	char *title;
	char *res;
	char *des;
	HTTP *http;

	url=url_encode(data->arg);
	http=http_head_init();
	buf=string_add("GET /Bing/SearchWeb/v1/Web?Query=%%27%s%%27&$top=1&$skip=0&$format=json HTTP/1.1\n",url);
	free(url);
	http_head_add(http,buf);
	free(buf);
	http_head_add(http,"Host: api.datamarket.azure.com\n");
	http_head_add(http,"Accept: */*\n");
	http_head_add(http,BING_AU);
	http_head_add(http,"Connection: close\n\n");

	buf=https_perform(http,"api.datamarket.azure.com",443,NULL,NULL);
	title=match_string("\"Title\":\".[^\"]*",buf);
	des=match_string("\"Description\":\".[^\"]*",buf);
	url=match_string("\"Url\":\".[^\"]*",buf);
	free(buf);

	res=string_add("%s<->%s-->%s",title+9,url+8,des+15);
	free(title);
	free(des);
	free(url);
	msg_send(res,data);
	smbot_destory(data);
}

void get_google_image_url(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	char *des;
	char *url;
	int i;

	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';

	buf=string_add("https://www.googleapis.com/customsearch/v1?key=%s&cx=006431901905483214390:i3yxhoqkzo0&num=1&q=%s&searchType=image",GOOGLE_KEY,data->arg);
	res=https_get_simple(buf,443);
	free(buf);

	url=match_string("\"link\": \".[^\"]*",res);
	des=match_string("\"snippet\": \".[^\"]*",res);
	free(res);
	res=string_add("%s<--%s",des+11,url+8);
	free(des);
	free(url);

	msg_send(res,data);
	free(res);
	smbot_destory(data);
}

void get_google(SMBOT_DATA *data)
{
	char *res;
	char *url;
	char *buf;
	char *des;
	int i;

	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';
	buf=string_add("https://www.googleapis.com/customsearch/v1?key=%s&cx=006431901905483214390:i3yxhoqkzo0&num=1&q=%s",GOOGLE_KEY,data->arg);
	res=https_get_simple(buf,443);
	free(buf);

	url=match_string("\"link\": \".[^\"]*",res);
	des=match_string("\"snippet\": \".[^\"]*",res);
	free(res);

	res=string_add("%s<--%s",url+8,des+11);
}

void get_baidu(SMBOT_DATA *data)
{
	char *buf;
	char *res;
	char temp[512];
	char *title;
	char *url;
	int i;

	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';

	buf=string_add("http://www.baidu.com/s?wd=%s&rsv_spt=1&issp=1&rsv_bp=0&ie=utf-8&tn=baidujson&rn=1",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	url=match_string("uri:\".[^\"]*",res);
	title=match_string("title:\".[^\"]*",res);
	buf=match_string("comment:\".[^\"]*",res);
	free(res);
	strreplace(buf+9,"\x1","",temp,512);
	free(buf);
	buf=string_add("%s",temp);
	bzero(temp,512);
	to_iconv("GB18030//","UTF-8//IGNORE",buf,strlen(buf),temp,512);
	free(buf);

	res=string_add("%s<->%s---%s",title,url,temp);
	free(title);
	free(url);

	msg_send(res,data);
	free(res);
	smbot_destory(data);
	
}

void get_bimg(SMBOT_DATA *data)
{
	char *title;
	char *url;
	char temp[512]={0};
	char *res;
	char *buf;
	int i;

	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';

	buf=string_add("http://image.baidu.com/i?tn=baiduimageson&ipn=r&ct=201326592&cl=2&lm=-1&st=-1&fm=result&fr=&sf=1&fmq=1376411397766_R&pv=&ic=0&nc=1&z=&se=1&showtab=0&fb=0&width=&height=&face=0&istype=2&ie=utf-8&word=%s&pn=1&rn=1",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	url=match_string("\"objURL\":\".[^\"]*",res);
	title=match_string("\"fromPageTitleEnc\":\".[^\"]*",res);
	free(res);
	to_iconv("GB18030//","UTF-8//IGNORE",title+20,strlen(title)-20,temp,512);
	free(title);
	res=string_add("%s<--%s",temp,url);
	free(url);
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
