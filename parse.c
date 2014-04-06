#include "parse.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *error_data(char *msg);
char *stradd(char *from,char *sub);
int object_stradd(char **res,char *str,char *key,json_object *obj);

char *youku_parse(char *str)
{
	char *res=NULL;
	json_object *obj;
	json_object *item;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回了错误的数据");
	obj=json_object_array_get_idx(obj,0);
	if(obj == NULL)
		return error_data("返回了错误的数据");

	item=json_object_object_get(obj,"title");
	res=stradd(NULL,json_object_get_string(item));
	json_object_put(item);

	res=stradd(res," <--");
	item=json_object_object_get(obj,"url");
	res=stradd(res,json_object_get_string(item));
	json_object_put(item);

	json_object_put(obj);

	return res;
}

char *google_parse(char *str)
{
	char *res=NULL;
	json_object *obj;
	json_object *item;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回了错误的数据!");
	obj=json_object_object_get(obj,"items");
	if(obj == NULL)
		return error_data("啊哦，查询出错了哦!");

	obj=json_object_array_get_idx(obj,0);
	if(obj == NULL)
		return error_data("啊哦，查询出错了哦!");
	item=json_object_object_get(obj,"title");
	if(item == NULL)
		return error_data("啊哦，查询出错了哦!");
	res=stradd(res,json_object_get_string(item));
	res=stradd(res," <-> ");
	json_object_put(item);

	item=json_object_object_get(obj,"link");
	res=stradd(res,json_object_get_string(item));
	res=stradd(res," ----");
	json_object_put(item);

	item=json_object_object_get(obj,"snippet");
	res=stradd(res,json_object_get_string(item));
	json_object_put(item);

	json_object_put(obj);

	return res;
}

char *bing_parse(char *str)
{
	json_object *obj;
	json_object *item;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("啊哦，返回的数据是错误的，所以淫家不能给你发送呢");
	obj=json_object_object_get(obj,"d");
	if(obj == NULL)
		return error_data("你在搜些什么，为什么我什么都没有发现!");
	obj=json_object_object_get(obj,"results");
	obj=json_object_array_get_idx(obj,0);
	if(obj == NULL)
		return error_data("你在搜些什么，为什么我什么都没有发现");

	item=json_object_object_get(obj,"Title");
	res=stradd(res,json_object_get_string(item));
	json_object_put(item);

	res=stradd(res," <-> ");
	item=json_object_object_get(obj,"Url");
	res=stradd(res,json_object_get_string(item));
	json_object_put(item);

	res=stradd(res," ----");
	item=json_object_object_get(obj,"Description");
	res=stradd(res,json_object_get_string(item));
	json_object_put(item);
	json_object_put(obj);

	return res;
}

char *dict_parse(char *str)
{
	json_object *obj;
	json_object *item;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回的数据是错误的!");
	obj=json_object_object_get(obj,"d");
	if(obj == NULL)
		return error_data("翻译出错!");
	obj=json_object_object_get(obj,"results");
	obj=json_object_array_get_idx(obj,0);
	if(obj == NULL)
		return error_data("翻译出错");

	item=json_object_object_get(obj,"Text");
	res=stradd(res,json_object_get_string(item));
	json_object_put(item);

	json_object_put(obj);

	return res;
}

char *weather_parse(char *str)
{
	json_object *obj;
	json_object *item;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回的数据是错误的!");

	item=json_object_object_get(obj,"resultcode");
	if(strcmp("200",json_object_get_string(item)))
	{
		json_object_put(item);
		json_object_put(obj);
		return error_data("啊哦，查询出错了哦!");
	}
	json_object_put(item);
	obj=json_object_object_get(obj,"result");

	item=json_object_object_get(obj,"sk");
	res=stradd(res,"实况天气: ");
	object_stradd(&res,"温度:","temp",item);
	object_stradd(&res," 风向:","wind_direction",item);
	object_stradd(&res," 风力:","wind_strength",item);
	object_stradd(&res," 湿度:","humidity",item);
	object_stradd(&res," 更新时间:","time",item);
	json_object_put(item);

	item=json_object_object_get(obj,"today");
	res=stradd(res,"  今日天气: ");
	object_stradd(&res,"城市:","city",item);
	object_stradd(&res," 日期:","date_y",item);
	object_stradd(&res," ","week",item);
	object_stradd(&res," 温度:","temperature",item);
	object_stradd(&res," 天气:","weather",item);
	object_stradd(&res," ","wind",item);
	object_stradd(&res," 穿衣指数:","dressing_index",item);
	object_stradd(&res," 穿衣建议:","dressing_advice",item);
	object_stradd(&res," 紫外线强度:","uv_index",item);
	object_stradd(&res," 洗车指数:","wash_index",item);
	object_stradd(&res," 旅游指数:","travel_index",item);
	object_stradd(&res," 晨练指数:","exercise_index",item);

	json_object_put(item);
	json_object_put(obj);

	return res;
}

char *wifi_parse(char *str)
{
	json_object *obj;
	json_object *item;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回了错误的数据!");
	item=json_object_object_get(obj,"resultcode");
	if(strcmp("200",json_object_get_string(item)))
	{
		json_object_put(item);
		json_object_put(obj);
		return error_data("查询出错了哦!");
	}
	json_object_put(item);
	obj=json_object_object_get(obj,"result");
	obj=json_object_object_get(obj,"data");
	obj=json_object_array_get_idx(obj,0);
	if(obj == NULL)
		return error_data("查询出错了哦!");
	if(object_stradd(&res,"名字:","name",obj))
	{
		json_object_put(obj);
		return res;
	}
	object_stradd(&res," 所在城市:","city",obj);
	object_stradd(&res," 所在省份:","province",obj);
	object_stradd(&res," 简介:","intro",obj);
	object_stradd(&res," 地址:","address",obj);
	object_stradd(&res," google经度:","google_lon",obj);
	object_stradd(&res," google纬度:","google_lat",obj);
	object_stradd(&res," baidu经度:","baidu_lon",obj);
	object_stradd(&res," baidu纬度:","baidu_lat",obj);

	json_object_put(obj);

	return res;
}

char *bt_parse(char *str)
{
	json_object *obj;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回了错误的数据");
	obj=json_object_array_get_idx(obj,0);
	if(obj == NULL)
		return error_data("没有发现结果");

	if(object_stradd(&res,NULL,"title",obj))
	{
		json_object_put(obj);
		return res;
	}
	object_stradd(&res," --- ","magnet",obj);
	object_stradd(&res," 大小: ","size",obj);
	object_stradd(&res," 文件数: ","files",obj);
	object_stradd(&res," 下载数: ","downloads",obj);
	object_stradd(&res," 添加时间: ","addtime",obj);
	object_stradd(&res," 发现时间: ","update",obj);
	object_stradd(&res," 虚假种子: ","fake",obj);

	json_object_put(obj);

	return res;
}

char *baidu_parse(char *str)
{
	json_object *obj;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("没有查询到任何结果肿么办");
	obj=json_object_object_get(obj,"feed");
	obj=json_object_object_get(obj,"entry");
	obj=json_object_array_get_idx(obj,0);
	if(obj == NULL)
		return error_data("没有查询到任何结果肿么办!");


	if(object_stradd(&res,NULL,"title",obj))
	{
		json_object_put(obj);
		return res;
	}
	object_stradd(&res," <-> ","url",obj);
	object_stradd(&res," --- ","abs",obj);
	
	json_object_put(obj);

	return res;
}

char *news_parse(char *str)
{
	json_object *obj;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("查询出错了哦");
	obj=json_object_object_get(obj,"feed");
	obj=json_object_object_get(obj,"entry");
	obj=json_object_array_get_idx(obj,0);
	if(obj == NULL)
		return error_data("查询出错了哦");

	if(object_stradd(&res,NULL,"title",obj))
	{
		json_object_put(obj);
		return res;
	}
	object_stradd(&res," <-> ","url",obj);
	object_stradd(&res," --- ","abs",obj);

	json_object_put(obj);

	return res;
}

char *error_data(char *msg)
{
	char *res;

	res=malloc(sizeof(char)*strlen(msg)+1);
	snprintf(res,sizeof(char)*strlen(msg)+1,"%s",msg);

	return res;
}

char *stradd(char *from,char *sub)
{
	char *res;

	if(sub == NULL)
		return from;

	if(from == NULL)
	{
		res=malloc(sizeof(char)*strlen(sub)+1);
		snprintf(res,sizeof(char)*strlen(sub)+1,"%s",sub);
	}
	else
	{
		res=malloc(sizeof(char)*(strlen(from)+strlen(sub))+1);
		snprintf(res,sizeof(char)*(strlen(from)+strlen(sub))+1,"%s%s",from,sub);
		free(from);
	}

	return res;
}

int object_stradd(char **res,char *str,char *key,json_object *obj)
{
	json_object *item;

	item=json_object_object_get(obj,key);
	if(item == NULL)
	{
		*res=stradd(*res,"啊哦，淫家没有发现任何结果!");
		return 1;
	}
	*res=stradd(*res,str);
	*res=stradd(*res,json_object_get_string(item));
	json_object_put(item);

	return 0;
}
