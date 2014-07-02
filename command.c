#include "command.h"

#define null_and_help() \
{\
	if(data->arg == NULL)\
	{\
		smbot_destory(data);\
		return;\
	}\
	if(!data->have_arg)\
	{\
		msg_send(data->arg,data);\
		smbot_destory(data);\
		return;\
	}\
}
#define CHARSIZE sizeof(char)

typedef struct
{
	int len;
	char *data;
}retdata;

void msg_send(const char *msg,SMBOT_DATA *data);
size_t get_data(char *ptr,size_t size,size_t nmemb,retdata *data);

void get_man_url(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
//	char *res;
	retdata ret;

	null_and_help();

	if((data->arg[0] <= '8' && data->arg[0] >= '1') && data->arg[1] == ' ')
		buf=string_add("http://brisk.eu.org/api/man.php?q=%s&n=%c",data->arg+2,data->arg[0]);
		//buf=string_add("term=%s&section=%c&submitted=1\n\n",
		//		data->arg+2,data->arg[0]);
	else if((data->arg[0] > '8' || data->arg[0] < '0') && data->arg[1] != ' ')
		buf=string_add("http://brisk.eu.org/api/man.php?q=%s",data->arg);
		//buf=string_add("term=%s&section=-1&submitted=1\n\n",
		//		data->arg);
        else /*if((data->arg[1] == '\0') || (data->arg[3] == ' '))*/
        {
                msg_send("你一定是输入方式不对!",data);
                smbot_destory(data);
                free(data->arg);

                return;
        }

	ret.len=0;
	ret.data=NULL;

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("啊哦，淫家连接远程服务器失败了〇。〇",data);
	else if(ret.len)
	{
		free(buf);
		buf=man_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);

/*	res=http_post_simple("http://www.linuxmanpages.com/search.php",
			80,buf);
	null_no_free(buf);
	if(res == NULL)
	{
		msg_send("哎哟喂，淫家连接远程服务器失败了〇。〇!",data);
		smbot_destory(data);
		free(data->arg);

		return;
	}

	buf=match_string("location: .[^\r]*",res);
	if(buf == NULL)
	{
		msg_send("哎呀呀，淫家木有发现目标!",data);
		smbot_destory(data);
		free(res);
		free(data->arg);

		return;
	}

	free(res);
	res=string_add("http://www.linuxmanpages.com%s",buf+10);
	free(buf);*/

	//msg_send(res,data);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_ip_addr(SMBOT_DATA *data)
{
	int pipefd[2];
	char temp[1024]={0};

	null_and_help();

	pipe(pipefd);
	if(fork() == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("exec/query_ip_addr","query_ip_addr",data->arg,NULL);
	}

	close(pipefd[1]);
	wait(NULL);
	read(pipefd[0],temp,sizeof(temp));

	msg_send(temp,data);
	smbot_destory(data);
	free(data->arg);
}

void bing_dict(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	char *res;
	retdata ret;
	int i;

	null_and_help();
	for(i=0;data->arg[i] != ' ';++i)
		if(data->arg[i] == '\0')
			break;
	if(data->arg[i] == '\0')
	{
		msg_send("哎哟，表酱紫吗，你的格式错误了啦^_^",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=strnstr(data->arg,i);
	url=url_encode(data->arg+i+1);
	ret.len=0;
	ret.data=NULL;
	res=string_add("https://api.datamarket.azure.com/Bing/MicrosoftTranslator/v1/Translate?Text=%%27%s%%27&To=%%27%s%%27&$skip=0&$top=1&$format=json",url,buf);
	free(url);
	free(buf);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,res);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_easy_setopt(curl,CURLOPT_HTTPAUTH,CURLAUTH_BASIC);
	curl_easy_setopt(curl,CURLOPT_USERPWD,BING_AU);
	if(curl_easy_perform(curl) != 0)
		msg_send("我那个XX艹，我竟然连接远程服务器出错了!",data);
	else if(ret.len)
	{
		free(res);
		res=dict_parse(ret.data);
		free(ret.data);
		msg_send(res,data);
	}

	curl_easy_cleanup(curl);
	null_no_free(res);
	smbot_destory(data);
	free(data->arg);

}

/*void bing_dict(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	char *url;
	HTTP *http;
	int i;

	null_and_help();
	for(i=0;data->arg[i]!= ' ';++i)
		if(data->arg[i] == '\0')
			break;
	if(data->arg[i] == '\0')
	{
		msg_send("哎哟，表酱紫吗，你的格式错误了啦^_^",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	buf=strnstr(data->arg,i);

	http=http_head_init();
	url=url_encode(data->arg+i+1);
	res=string_add("GET /Bing/MicrosoftTranslator/v1/Translate?Text=%%27%s%%27&To=%%27%s%%27&$skip=0&$top=1&$format=json HTTP/1.1\n",url,buf);
	free(url);
	null_no_free(buf);
	http_head_add(http,res);
	//null_no_free(res);
	http_head_add(http,"Host: api.datamarket.azure.com\n");
	http_head_add(http,BING_AU);*/
//	http_head_add(http,"Accept: */*\n");
/*	http_head_add(http,"Connection: close\n\n");

	url=https_perform(http,"api.datamarket.azure.com",443,NULL,NULL);
	free(res);
	if(url == NULL)
	{
		msg_send("我那个XX艹，我竟然连接远程服务器出错了!",data);
		smbot_destory(data);
		http_head_destroy(http);
		free(data->arg);
		return;
	}

	buf=match_string("\"Text\":.[^\"]*",url);
	null_no_free(url);
	if(buf == NULL)
	{
		msg_send("啊哦，淫家木有发现结果肿么办!",data);
		smbot_destory(data);
		http_head_destroy(http);
		free(data->arg);
		return;
	}

	res=strnstr(buf+8,strlen(buf)-8);
	null_no_free(buf);
	msg_send(res,data);
	null_no_free(res);
	smbot_destory(data);
	http_head_destroy(http);
	free(data->arg);
}*/

void get_youku_url(SMBOT_DATA *data)
{
	CURL *curl;
	retdata ret;
	char *res;
	char *buf;
	char *url;
	
	null_and_help();
	url=url_encode(data->arg);
	buf=string_add("http://brisk.eu.org/api/youku.php?q=%s",url);
	free(url);

	curl=curl_easy_init();
	ret.data=NULL;
	ret.len=0;

	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("连接网络出现错误了哦",data);
	else if(ret.len)
	{
		res=youku_parse(ret.data);
		msg_send(res,data);
		free(ret.data);
		null_no_free(res);
	}
	else
		msg_send("哎呀，俺什么也没有查到!",data);

	curl_easy_cleanup(curl);
	smbot_destory(data);
	free(data->arg);
	free(buf);
}

void get_youtube(SMBOT_DATA *data)
{
	char *res;
	char *url;
	char *buf;
	char *des;
	HTTP *http;
	int i;

	null_and_help();
	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';

	buf=string_add("GET https://www.googleapis.com/youtube/v3/search?key=%s&part=snippet&maxResults=1&q=%s HTTP/1.1\n",GOOGLE_KEY,data->arg);
	http=http_head_init();
	http_head_add(http,buf);
	http_head_add(http,"Accept: */*\n");
	http_head_add(http,"Connect: close\n\n");
	res=http_perform(http,"127.0.0.1",8087);
	http_head_destroy(http);
	free(buf);

	if(res == NULL)
	{
		msg_send("淫家连接远程服务器失败了!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("\"videoId\": \".[^\"]*",res);
	if(buf == NULL)
	{
		null_no_free(res);
		msg_send("淫家没有找到肿么办!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	url=string_add("http://www.youtube.com/watch?v=%s",buf+12);
	free(buf);
	des=match_string("\"description\": \".[^\n]*",res);
	buf=match_string("\"title\": \".[^\n]*",res);
	free(res);

	i=strlen(des);
	des[i-2]='\0';
	i=strlen(buf);
	buf[i-2]='\0';

	res=string_add("%s -- %s -- %s",buf+10,des+16,url);
	free(buf);
	free(des);
	free(url);

	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}

void get_bt(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://brisk.eu.org/api/magnet.php?q=%s",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("我做出了一个艰难的决定，我要告诉你我连接远程服务器失败了!",data);
	else if(ret.len)
	{
		free(buf);
		buf=bt_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

/*void get_bt(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	char *head;
	HTTP *http;

	null_and_help();
	buf=url_encode(data->arg);
	head=string_add("GET http://btdigg.org/search?info_hash=&q=%s HTTP/1.1\n",buf);
	free(buf);
	http=http_head_init();
	http_head_add(http,head);
	http_head_add(http,"Host: btdigg.org\n");*/
//	http_head_add(http,"Accept: */*\n");
/*	http_head_add(http,"Connection: close\n\n");

	buf=http_perform(http,"127.0.0.1",8087);
	http_head_destroy(http);
	if(buf == NULL)
	{
		msg_send("我做出了一个艰难的决定，我要告诉你我连接远程服务器失败了!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	res=match_string("magnet:\?xt=urn:btih:*.[^\"]*",buf);

	if(res == NULL)
	{
		msg_send("你一定是打开方式不对，不然怎么会没有搜到结果呢!",data);
		smbot_destory(data);
		return;
	}

	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}*/

void get_zip_code(SMBOT_DATA *data)
{
	char *res;
	char code[80]={0};
	char *buf;
	char *temp;

	null_and_help();
	to_iconv("UTF-8//","GBK//",data->arg,strlen(data->arg),
			code,80);
	buf=string_add("http://opendata.baidu.com/post/s?wd=%s&p=mini&rn=20",code);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("呜~淫家连接远程服务器失败了!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=strstr(res,"href=\'s?wd=");
	if(buf == NULL)
	{
		msg_send("俺没有发现目标@.@",data);
		free(res);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	temp=match_string("[0-9]\\{6\\}[^<]*",buf);
	//free(buf);
	if(temp == NULL)
	{
		msg_send("俺没有发现目标!",data);
		free(res);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	
	free(res);
	msg_send(temp,data);
	free(temp);
	smbot_destory(data);
	free(data->arg);
}

void get_weather(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://v.juhe.cn/weather/index?key=your key&cityname=%s",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("我连啊连连啊连，对不起我没连上!",data);
	else if(ret.len)
	{
		free(buf);
		buf=weather_parse(ret.data);
		free(ret.data);
		msg_send(buf,data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

/*void get_weather(SMBOT_DATA *data)
{
	char *res;
	char *buf;

	null_and_help();
	buf=string_add("http://v.juhe.cn/weather/index?cityname=%s&key=your api key",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("我连啊连连啊连，对不起我没连上!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	if(strstr(res,"resultcode\":\"202\""))
	{
		free(res);
		msg_send("淫家木有查询到该城市信号了啦!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("temperature.*uv_index",res);
	free(res);
	if(buf == NULL)
	{
		msg_send("哎呀，俺出错了!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	res=strnstr(buf,-11);
	free(buf);
	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}*/

void get_stack(SMBOT_DATA *data)
{
	char *url;
	char *des;
	char *res;
	char *buf;
	int i;

	null_and_help();
	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';
	buf=string_add("http://stackoverflow.com/search?q=%s",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("俺连啊连连啊连，对不起俺没连上!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=strstr(res,"<div class=\"result-link\">");
	if(buf == NULL)
	{
		msg_send("淫家木有发现目标了啦!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	url=match_string("/questions.[^\"]*",buf);
	des=match_string("Q: .[^<]*",buf);
	free(res);

	if(url == NULL || des == NULL)
	{
		msg_send("啊哦，出了点小问题哦!",data);
		smbot_destory(data);
		null_no_free(url);
		null_no_free(des);
		free(data->arg);
		return;
	}

	res=string_add("%s <-- http://stackoverflow.com%s",des,url);
	free(url);
	free(des);
	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}

void get_id_information(SMBOT_DATA *data)
{
	char *res;
	char *buf;

	null_and_help();
	buf=string_add("http://apis.juhe.cn/idcard/index?key=your api key&cardno=%s",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("俺连啊连连啊连，哎呀，淫家木有连上!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("area.[^{]*",res);
	free(res);
	if(buf == NULL)
	{
		msg_send("出错了啦，你一定是输入的方式不对!",data);
		free(data->arg);
		smbot_destory(data);
		return;
	}

	msg_send(buf,data);
	free(buf);
	smbot_destory(data);
	free(data->arg);
}

void check_id_card(SMBOT_DATA *data)
{
	char temp[1024]={0};
	int pipefd[2];

	null_and_help();
	pipe(pipefd);
	
	if(fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		dup2(pipefd[1],STDERR_FILENO);

		execl("exec/id_card_test","id_card_test",data->arg,NULL);
	}

	close(pipefd[1]);
	wait(NULL);
	read(pipefd[0],temp,sizeof(temp));

	msg_send(temp,data);
	smbot_destory(data);
	free(data->arg);
}

void get_url_encode(SMBOT_DATA *data)
{
	char *url;

	null_and_help();
	url=url_encode(data->arg);
	msg_send(url,data);
	free(url);
	smbot_destory(data);
	free(data->arg);
}

void get_url_decode(SMBOT_DATA *data)
{
	char *res;

	null_and_help();
	res=url_decode(data->arg);
	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}

void get_joke(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	char *title=NULL;
	char *url=NULL;
	char temp[1024]={0};
	int i;

	null_and_help();
	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';

	buf=string_add("http://www.xiao688.com/cms/search/?sw=%s&sbtn=提交",data->arg);
	res=http_get_simple(buf,80);
	free(buf);

	if(res == NULL)
	{
		msg_send("没连上没连上没连上!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("<div class=\"title\">\\[<a href=\".[^\n]*",res);
	if(buf == NULL)
	{
		msg_send("啊哦，你在搜些什么，为什么我什么都没搜到!",data);
		smbot_destory(data);
		free(data->arg);
		free(res);
		return;
	}

	title=match_string("title=\".[^\"]*",buf);
	url=match_string("\] <a href=\".[^\"]*",buf);
	
	if(title == NULL || url == NULL)
	{
		msg_send("啊哦，你在搜些什么，为什么我什么都没搜到!",data);
		smbot_destory(data);
		free(data->arg);
		null_no_free(buf);
		null_no_free(title);
		null_no_free(url);
		free(res);
		return;
	}

	buf=match_string("<div class=\"des\">.[^\n]*",res);
	free(res);
	if(buf == NULL)
	{
		msg_send("啊哦，你在搜些什么，为什么我什么都没搜到!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	res=strnstr(buf+strlen("<div class=\"des\">"),-7);
	free(buf);
	if(res == NULL)
		goto send;

	strreplace(res,"<b>","",temp,sizeof(temp));
	free(res);
	res=string_add("%s",temp);
	bzero(temp,sizeof(temp));
	strreplace(res,"</b>","",temp,sizeof(temp));
	free(res);

	if(strstr(temp,"&nbsp;"))
	{
		res=string_add("%s",temp);
		bzero(temp,sizeof(temp));
		strreplace(res,"&nbsp;","",temp,sizeof(temp));
		free(res);
	}

send:
	buf=string_add("%s -->http://www.xiao688.com%s <--%s\n",title+strlen("title=\""),
			url+strlen("\] <a href=\""),temp);

	msg_send(buf,data);
	free(buf);
	smbot_destory(data);
	free(data->arg);
}

/*void get_dream(SMBOT_DATA *data)
{
	char *res;
	char *url;
	char *buf;
	
	null_and_help();

	url=url_encode(data->arg);
	buf=string_add("http://api2.sinaapp.com/search/dream/?appkey=0020130430&appsecert=fa6095e113cd28fd&reqtype=text&keyword=%s",url);
	free(url);

	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("额，那个什么，俺没连上",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("content\":\".[^\}]*",res);
	if(buf == NULL)
	{
		msg_send("查询出了点小小滴问题",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	free(res);
	res=strnstr(buf+strlen("content\":\""),-1);
	free(buf);

	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}*/

void get_dream(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://brisk.eu.org/api/dream.php?dream=%s",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);

	if(curl_easy_perform(curl) != 0)
		msg_send("哎呀，连接网络出现错误了哦!",data);
	else if(ret.len)
	{
		free(buf);
		buf=dream_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_song_url(SMBOT_DATA *data)
{
	char *buf;
	char *res;
	char url[512];
	char *code;

	null_and_help();
	code=url_encode(data->arg);
	buf=string_add("http://music.baidu.com/search?key=%s",code);
	free(code);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("啊哦，淫家连接远程服务器失败了!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("<a href=\"/song/.[^\"]*",res);
	free(res);

	if(buf == NULL)
	{
		msg_send("啊哦，淫家木有发现结果肿么办〇。〇",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	strreplace(buf,"<a href=\"","http://music.baidu.com",url,512);
	free(buf);
	
	msg_send(url,data);
	smbot_destory(data);
	free(data->arg);
}

void get_bing(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);

	ret.data=NULL;
	ret.len=0;
	buf=string_add("https://api.datamarket.azure.com/Bing/SearchWeb/v1/Web?Query=%%27%s%%27&$top=1&$skip=0&$format=json",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_easy_setopt(curl,CURLOPT_HTTPAUTH,CURLAUTH_BASIC);
	curl_easy_setopt(curl,CURLOPT_USERPWD,BING_AU);
	if(curl_easy_perform(curl) != 0)
		msg_send("俺逢山过水，遇神杀神遇佛杀佛。。。好吧，淫家没连上远程服务器= =",data);
	else if(ret.len)
	{
		free(buf);
		buf=bing_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

/*void get_bing(SMBOT_DATA *data)
{
	char *url;
	char *buf;
	char *title;
	char *res;
	char *des;
	HTTP *http;

	null_and_help();
	url=url_encode(data->arg);
	http=http_head_init();
	res=string_add("GET /Bing/SearchWeb/v1/Web?Query=%%27%s%%27&$top=1&$skip=0&$format=json HTTP/1.1\n",url);
	free(url);
	http_head_add(http,res);
	//free(buf);
	http_head_add(http,"Host: api.datamarket.azure.com\n");*/
//	http_head_add(http,"Accept: */*\n");
/*	http_head_add(http,BING_AU);
	http_head_add(http,"Connection: close\n\n");

	buf=https_perform(http,"api.datamarket.azure.com",443,NULL,NULL);
	free(res);
	if(buf == NULL)
	{
		msg_send("俺逢山过水，遇神杀神遇佛杀佛。。。好吧，淫家没连上远程服务器!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	title=match_string("\"Title\":\".[^\"]*",buf);
	des=match_string("\"Description\":\".[^\"]*",buf);
	url=match_string("\"Url\":\".[^\"]*",buf);
	free(buf);
	
	if(title == NULL || des == NULL || url == NULL)
	{
		msg_send("呜~淫家木有发现结果肿么办",data);
		smbot_destory(data);
		null_no_free(data->arg);
		return;
	}

	res=string_add("%s<->%s -->%s",title+9,url+7,des+15);
	free(title);
	free(des);
	free(url);
	msg_send(res,data);
	smbot_destory(data);
	free(data->arg);
}*/

void get_google_image_url(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	char *des;
	char *url;
	HTTP *http;
	int i;

	null_and_help();
	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';

	buf=string_add("GET https://www.googleapis.com/customsearch/v1?key=%s&cx=006431901905483214390:i3yxhoqkzo0&num=1&q=%s&searchType=image HTTP/1.1\n",GOOGLE_KEY,data->arg);
	/*res=https_get_simple(buf,443);
	free(buf);*/
	http=http_head_init();
	http_head_add(http,buf);
	http_head_add(http,"Accept: */*\n");
	http_head_add(http,"Connection: close\n\n");
	res=http_perform(http,"127.0.0.1",8087);
	http_head_destroy(http);
	free(buf);

	if(res == NULL)
	{
		msg_send("俺一定是打开方式不对，不然怎么会没有搜到结果呢!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	url=match_string("\"link\": \".[^\"]*",res);
	des=match_string("\"snippet\": \".[^\n]*",res);
	free(res);
	if(url == NULL || des == NULL)
	{
		msg_send("俺木有找到你想要的东西!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	i=strlen(des);
	des[i-2]='\0';

	res=string_add("%s<--%s",des+12,url+9);
	free(des);
	free(url);

	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}

void get_google(SMBOT_DATA *data)
{
	CURL *curl;
	retdata ret;
	char *buf;
	char *url;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("https://www.googleapis.com/customsearch/v1?key=%s&cx=006431901905483214390:i3yxhoqkzo0&num=1&q=%s",GOOGLE_KEY,url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0);
	//curl_easy_setopt(curl,CURLOPT_PROXY,"127.0.0.1:8087");
	if(curl_easy_perform(curl) != 0)
		msg_send("连接网络出现错误了哦!",data);
	else if(ret.len)
	{
		free(buf);
		buf=google_parse(ret.data);
		free(ret.data);
		msg_send(buf,data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_baidu(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://www.baidu.com/s?wd=%s&rsv_spt=1&issp=1&rsv_bp=0&ie=utf-8&tn=json&rn=1",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("哎呀，连接到远程服务器出错了〇。〇",data);
	else if(ret.len)
	{
		free(buf);
		buf=baidu_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

/*void get_baidu(SMBOT_DATA *data)
{
	char *buf;
	char *res;
	char temp[512]={0};
	char ti[512]={0};
	char *title;
	char *url;
	int i;

	null_and_help();
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

	if(url == NULL || title == NULL || buf == NULL)
	{
		msg_send("肿么办，淫家什么也没查到!",data);
		smbot_destory(data);
		null_no_free(data->arg);
		return;
	}

	strreplace(buf+9,"\x1","",temp,512);
	free(buf);
	buf=string_add("%s",temp);
	bzero(temp,512);
	to_iconv("GB18030//","UTF-8//IGNORE",buf,strlen(buf),temp,512);
	free(buf);
	to_iconv("GB18030//","UTF-8//IGNORE",title+6,strlen(title),ti,512);
	free(title);

	res=string_add("%s<->%s ---%s",ti,url+5,temp);
	free(url);

	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}*/

void get_bimg(SMBOT_DATA *data)
{
	char *title;
	char *url;
	char temp[512]={0};
	char *res;
	char *buf;
	int i;

	null_and_help();
	for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';

	buf=string_add("http://image.baidu.com/i?tn=baiduimagejson&ipn=r&ct=201326592&cl=2&lm=-1&st=-1&fm=result&fr=&sf=1&fmq=1376411397766_R&pv=&ic=0&nc=1&z=&se=1&showtab=0&fb=0&width=&height=&face=0&istype=2&ie=utf-8&word=%s&pn=1&rn=1",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	url=match_string("\"objURL\":\".[^\"]*",res);
	title=match_string("\"fromPageTitleEnc\":\".[^\"]*",res);
	free(res);
	if(url == NULL || title == NULL)
	{
		null_no_free(url);
		null_no_free(title);
		msg_send("哎呀呀，淫家什么也没查到!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	to_iconv("GB18030//","UTF-8//IGNORE",title+20,strlen(title)-20,temp,512);
	if(strstr(temp,"<strong>"))
	{
		buf=string_add("%s",temp);
		bzero(temp,sizeof(temp));
		strreplace(buf,"<strong>","",temp,sizeof(temp));
		free(buf);
		buf=string_add("%s",temp);
		bzero(temp,sizeof(temp));
		strreplace(buf,"<\\/strong>","",temp,sizeof(temp));
		free(buf);
	}

	free(title);
	res=string_add("%s<--%s",temp,url+10);
	free(url);
	msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}

void get_news(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://news.baidu.com/ns?word=%s&tn=json&rn=1",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("啊哦，淫家连接远程服务器失败了!",data);
	else if(ret.len)
	{
		free(buf);
		buf=news_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_air(SMBOT_DATA *data)
{
	char *buf;
	char *res;

	null_and_help();
	buf=string_add("http://web.juhe.cn/environment/air/cityair?city=%s&key=your api key",data->arg);
	res=http_get_simple(buf,8080);
	free(buf);
	if(res == NULL)
	{
		msg_send("啊，淫家连接远程服务器失败了!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	if(!strstr(res,"SUCCESSED!"))
	{
		free(res);
		msg_send("查询失败了，你一定是没有用拼音!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("\"city\":\".[^}]*",res);
	free(res);
	msg_send(buf,data);
	free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_website_testing(SMBOT_DATA *data)
{
	char *res;
	char *buf;

	null_and_help();
	buf=string_add("http://apis.juhe.cn/webscan/?domain=%s&key=your api key",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("啊呀，淫家连接远程服务器失败了!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	if(!strstr(res,"Successed!"))
	{
		free(res);
		msg_send("检测失败了啦!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	buf=match_string("{\"state\":.[^\)]*",res);
	free(res);
	msg_send(buf,data);
	free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_wifi(SMBOT_DATA *data)
{
	CURL *curl;
	char *url;
	char *buf;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://apis.juhe.cn/wifi/region?key=your key&city=%s",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("哎呀呀，淫家连接远程服务器失败了!",data);
	else if(ret.len)
	{
		free(buf);
		buf=wifi_parse(ret.data);
		free(ret.data);
		msg_send(buf,data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

/*void get_wifi(SMBOT_DATA *data)
{
	char *buf;
	char *res;

	null_and_help();
	res=url_encode(data->arg);
	buf=string_add("http://apis.juhe.cn/wifi/region?key=your api key&city=%s",res);
	free(res);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("哎呀呀，淫家连接远程服务器失败了!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	if(!strstr(res,"Return Successd!"))
	{
		free(res);
		msg_send("淫家木有发现结果肿么办!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("\"name\":\".[^}]*",res);
	free(res);
	msg_send(buf,data);
	free(buf);
	smbot_destory(data);
	free(data->arg);
}*/

void get_train(SMBOT_DATA *data)
{
	char *res;
	char *buf;

	null_and_help();
	buf=string_add("http://apis.juhe.cn/train/s?name=%s&key=your api key",data->arg);
	res=http_get_simple(buf,80);
	free(buf);
	if(res == NULL)
	{
		msg_send("俺连啊连连啊连，对不起俺没连上!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	if(!strstr(res,"Successed!"))
	{
		free(res);
		msg_send("俺查啊查，查啊查，对不起，俺没什么也没查到!",data);
		smbot_destory(data);
		free(data->arg);
		return;
	}
	buf=match_string("\"name\":\".[^}]*",res);
	free(res);
	msg_send(buf,data);
	smbot_destory(data);
	free(data->arg);
}

void get_sm_message(SMBOT_DATA *data)
{
	char *res;
	char *buf;
	char *url;
	char temp[1024];
	int i;

	null_and_help();
	/*for(i=0;data->arg[i];++i)
		if(data->arg[i] == ' ')
			data->arg[i]='+';*/

	url=url_encode(data->arg);
	buf=string_add("http://xiaofengrobot.sinaapp.com/web.php?callback=jQuery191041205509454157474_1376842442554&para=%s&_=1376842442555",url);
	free(url);

	while(1)
	{
		res=http_get_simple(buf,80);
		if(res == NULL)
			continue;
		if(res[0] <= '\0')
		{
			null_no_free(res);
			continue;
		}
		else
			break;
	}
	free(buf);

	if(strstr(res,"504 Gateway Time-out") && strstr(res,"503 Service Unavailable"))
	{
		msg_send("哎呀，淫家有点不方便了啦，每个月总会有那么几天，放过俺先吧^_^",data);
		free(res);
		smbot_destory(data);
		free(data->arg);
		return;
	}

	buf=match_string("test(\".[^\n]*",res);
	free(res);
	if(buf == NULL)
	{
		smbot_destory(data);
		free(data->arg);
		return;
	}
	i=strlen(buf);
	buf[i-3]='\0';

	res=unicode_to_utf(buf+6);
	free(buf);

	if(strreplace(res,"<br>","",temp,1023) != -1)
		msg_send(temp,data);
	else
		msg_send(res,data);
	free(res);
	smbot_destory(data);
	free(data->arg);
}

void get_word(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://brisk.eu.org/api/xhzd.php?word=%s",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("啊哦，淫家连接远程服务器失败了!",data);
	else if(ret.len)
	{
		free(buf);
		buf=word_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_term(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://brisk.eu.org/api/hycd.php?word=%s",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("啊哦，淫家连接远程服务器失败了!",data);
	else if(ret.len)
	{
		free(buf);
		buf=term_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_idiom(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://brisk.eu.org/api/cycd.php?word=%s",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("啊哦，淫家连接远程服务器失败了!",data);
	else if(ret.len)
	{
		free(buf);
		buf=idiom_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

void get_base64_encode(SMBOT_DATA *data)
{
	char *buf;

	null_and_help();
	buf=base64_encode(data->arg,strlen(data->arg));
	msg_send(buf,data);

	smbot_destory(data);
	free(buf);
	free(data->arg);
}

void get_base64_decode(SMBOT_DATA *data)
{
	char *buf=NULL;

	null_and_help();
	if(strlen(data->arg)%4 != 0)
		msg_send("你输入的可能不是base64字符串哦!",data);
	else if(strstr(data->arg,"=") && data->arg[strlen(data->arg)-1] != '=')
		msg_send("你输入的可能不是base64字符串哦!",data);
	else
	{
		buf=base64_decode(data->arg,strlen(data->arg));
		msg_send(buf,data);
	}

	smbot_destory(data);
	null_no_free(buf);
	free(data->arg);
}

void get_baike(SMBOT_DATA *data)
{
	CURL *curl;
	char *buf;
	char *url;
	retdata ret;

	null_and_help();
	url=url_encode(data->arg);
	ret.len=0;
	ret.data=NULL;
	buf=string_add("http://brisk.eu.org/api/baike.php?q=%s",url);
	free(url);

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,buf);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	if(curl_easy_perform(curl) != 0)
		msg_send("啊哦，淫家连接远程服务器失败了!",data);
	else if(ret.len)
	{
		free(buf);
		buf=baike_parse(ret.data);
		msg_send(buf,data);
		free(ret.data);
	}

	curl_easy_cleanup(curl);
	null_no_free(buf);
	smbot_destory(data);
	free(data->arg);
}

void msg_send(const char *msg,SMBOT_DATA *data)
{
	if(data->is_use_ssl)
		ssl_msgto(ssl,data->channel,data->nick,msg);
	else
		msgto(sockfd,data->channel,data->nick,msg);
}

size_t get_data(char *ptr,size_t size,size_t nmemb,retdata *data)
{
	if(data->len)
	{
		char *temp;

		temp=malloc(CHARSIZE*data->len+1);
		snprintf(temp,CHARSIZE*data->len+1,"%s",data->data);
		free(data->data);

		data->data=malloc(CHARSIZE*(data->len+nmemb)+1);
		snprintf(data->data,CHARSIZE*(data->len+nmemb)+1,"%s%s",temp,ptr);
		free(temp);
	}
	else
	{
		data->data=malloc(CHARSIZE*nmemb+1);
		snprintf(data->data,CHARSIZE*nmemb+1,"%s",ptr);
	}

	data->len+=nmemb;

	return nmemb;
}
