#include "parse.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *error_data(char *msg);
char *stradd(char *from,char *sub);
int object_stradd(char **res,char *str,char *key,json_object *obj);
char *website_error(int code);

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

/*char *google_parse(char *str)
{
	char *res=NULL;
	json_object *obj;
	json_object *item;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回了错误的数据!");
	obj=json_object_object_get(obj,"items");
	if(obj == NULL)
		return error_data("啊哦，查询出错了哦!(可能是当日使用量已过百，请明日再试，具体事宜请联系brisk)");

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
}*/
char *google_parse(char *str)
{
	json_object *obj;
	json_object *items;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回的数据是错误的!");

	items=json_object_array_get_idx(obj,0);
	if(items == NULL)
	{
		json_object_put(obj);
		return error_data("没有查询到结果!");
	}

	object_stradd(&res,"","title",items);
	object_stradd(&res," <-> ","url",items);
	object_stradd(&res," ----","descrition",items);

	json_object_put(items);
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
	json_object *weather;
	json_object *weather_temp;
	json_object *temp;
	int i,len;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回的数据是错误的!");

	item=json_object_object_get(obj,"error");
	if(json_object_get_int(item) != 0)
	{
		json_object_put(item);
		object_stradd(&res,"查询出错:","status",obj);
		json_object_put(obj);
		
		return res;
	}
	json_object_put(item);

	object_stradd(&res,"日期:","date",obj);
	weather=json_object_object_get(obj,"results");
	item=json_object_array_get_idx(weather,0);
	object_stradd(&res," 城市:","currentCity",item);
	object_stradd(&res," pm25:","pm25",item);

	weather_temp=json_object_object_get(item,"weather_data");
	len=json_object_array_length(weather_temp);
	for(i=0;i < len;++i)
	{
		temp=json_object_array_get_idx(weather_temp,i);

		object_stradd(&res," 日期:","date",temp);
		object_stradd(&res," 天气:","weather",temp);
		object_stradd(&res," 风力:","wind",temp);
		object_stradd(&res," 温度:","temperature",temp);

		json_object_put(temp);
	}
	json_object_put(weather_temp);

	weather_temp=json_object_object_get(item,"index");
	len=json_object_array_length(weather_temp);
	for(i=0;i < len;++i)
	{
		temp=json_object_array_get_idx(weather_temp,i);

		object_stradd(&res," ","tipt",temp);
		object_stradd(&res,":","des",temp);

		//json_object_put(temp);
	}
	json_object_put(weather_temp);

	json_object_put(item);
	json_object_put(weather);
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

char *word_parse(char *str)
{
	json_object *obj;
	json_object *res;
	json_object *item;
	int len;
	int i;
	char *text=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data(str);
	res=json_object_object_get(obj,"res");
	if(res == NULL)
	{
		json_object_put(obj);
		return error_data("查询出错了哦!");
	}
	
	len=json_object_array_length(res);
	if(len == 0)
	{
		json_object_put(res);
		json_object_put(obj);
		return error_data("查询出错了哦!");
	}

	for(i=0;i < len;++i)
	{
		item=json_object_array_get_idx(res,i);

		object_stradd(&text," 字:","word",item);
		object_stradd(&text," 解释:","text",item);

		json_object_put(item);
	}

	json_object_put(res);
	json_object_put(obj);

	return text;
}

char *term_parse(char *str)
{
	json_object *obj;
	json_object *res;
	json_object *item;
	int len;
	int i;
	char *text=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data(str);
	res=json_object_object_get(obj,"res");
	if(res == NULL)
	{
		json_object_put(obj);
		return error_data("查询出错了哦!");
	}

	len=json_object_array_length(res);
	if(len == 0)
	{
		json_object_put(res);
		json_object_put(obj);
		return error_data("查询出错了哦!");
	}

	for(i=0;i < len;++i)
	{
		item=json_object_array_get_idx(res,i);

		object_stradd(&text," 词:","word",item);
		object_stradd(&text," 解释:","text",item);

		json_object_put(item);
	}

	json_object_put(res);
	json_object_put(obj);

	return text;
}

char *idiom_parse(char *str)
{
	json_object *obj;
	json_object *res;
	json_object *item;
	char *text=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data(str);
	res=json_object_object_get(obj,"res");
	if(res == NULL)
	{
		json_object_put(obj);
		return error_data("查询出错了哦!");
	}

	item=json_object_array_get_idx(res,0);
	object_stradd(&text,"成语:","word",item);
	object_stradd(&text," 解释:","text",item);
	json_object_put(item);

	json_object_put(res);
	json_object_put(obj);

	return text;
}

char *dream_parse(char *str)
{
	json_object *obj;
	json_object *res;
	json_object *item;
	json_object *dream;
	char *text=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("返回了错误的数据!");
	res=json_object_object_get(obj,"dreams");
	if(res == NULL)
	{
		json_object_put(obj);
		return error_data("周公无法解此梦!");
	}
	
	item=json_object_array_get_idx(res,0);
	dream=json_object_object_get(item,"description");
	text=strdup(json_object_get_string(dream));

	json_object_put(dream);
	json_object_put(item);
	json_object_put(res);
	json_object_put(obj);

	return text;
}

char *man_parse(char *str)
{
	json_object *obj;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("查询出错了哦!");

	if(object_stradd(&res," ","url",obj))
	{
		json_object_put(obj);
		return res;
	}
	object_stradd(&res," <--> ","name",obj);
	object_stradd(&res," <-> synopsis: ","synopsis",obj);
	object_stradd(&res," <----- ","description",obj);

	json_object_put(obj);

	return res;
}

char *baike_parse(char *str)
{
	json_object *obj;
	json_object *baike;
	json_object *items;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL)
		return error_data("查询出错了哦!");

	baike=json_object_object_get(obj,"baike");
	if(baike == NULL)
	{
		json_object_put(obj);
		return error_data("没有查询到任何结果肿么办!");
	}

	items=json_object_array_get_idx(baike,0);
	object_stradd(&res," ","url",items);
	object_stradd(&res," <-> ","title",items);
	object_stradd(&res," ----- ","des",items);

	json_object_put(items);
	json_object_put(baike);
	json_object_put(obj);

	return res;
}

char *train_parse(char *str)
{
	json_object *obj;
	json_object *item;
	json_object *train;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(obj == NULL) return error_data("查询出错了哦!");

	item=json_object_object_get(obj,"trainInfo");
	if(!item)
	{
		json_object_put(obj);
		return error_data("无法查询到该车次的信息!");
	}

	train=(json_object *)(json_object_get_object(item)->head)->v;
	object_stradd(&res," 车次:","code",train);
	object_stradd(&res," 起始站:","deptCity",train);
	object_stradd(&res," 终点站:","arriCity",train);
	object_stradd(&res," 列车类型:","trainType",train);
	object_stradd(&res," 开车时间:","deptTime",train);
	object_stradd(&res," 到站时间:","arriTime",train);
	object_stradd(&res," 全程时间:","interval",train);
	json_object_put(train);
	json_object_put(item);

	item=json_object_object_get(obj,"extInfo");
	object_stradd(&res," 全程长度:","allMileage",item);
	json_object_put(item);

	json_object_put(obj);
	return res;
}

char *website_parse1(char *str)
{
	json_object *obj;
	json_object *item;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(!obj)
		return error_data("检测钓鱼: 查询出错!");
	item=json_object_object_get(obj,"success");
	if(json_object_get_int(item) == 0)
	{
		json_object_put(item);
		item=json_object_object_get(obj,"errno");
		res=website_error(json_object_get_int(item));

		json_object_put(item);
		json_object_put(obj);
		return res;
	}
	json_object_put(item);

	item=json_object_object_get(obj,"phish");
	switch(json_object_get_int(item))
	{
		case -1:
			res="钓鱼: 未知";
			break;
		case 0:
			res="钓鱼: 非钓鱼";
			break;
		case 1:
			res="钓鱼: 钓鱼";
			break;
		case 2:
			res="钓鱼: 高风险，有钓鱼嫌疑";
			break;
		default:
			res="钓鱼: 不可测";
	}

	json_object_put(item);
	json_object_put(obj);

	return error_data(res);
}

char *website_parse2(char *str)
{
	json_object *obj;
	json_object *item;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(!obj)
		return error_data("检测下载连接: 查询出错!");
	item=json_object_object_get(obj,"success");
	if(json_object_get_int(item) == 0)
	{
		json_object_put(item);
		item=json_object_object_get(obj,"errno");
		res=website_error(json_object_get_int(item));

		json_object_put(item);
		json_object_put(obj);
		return res;
	}
	json_object_put(item);

	item=json_object_object_get(obj,"down_type");
	switch(json_object_get_int(item))
	{
		case 1:
			res="链接安全: 未知";
			break;
		case 2:
			res="链接安全: 安全";
			break;
		case 3:
			res="链接安全: 危险";
			break;
		case 6:
			res="链接安全: 非PE文件";
			break;
		default:
			res="链接安全: 不可测";
	}

	json_object_put(item);
	json_object_put(obj);
	return error_data(res);
}

char *id_information_parse(char *str)
{
	json_object *obj;
	json_object *item;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(!obj)
		return error_data("查询出错了哦!");
	item=json_object_object_get(obj,"error");
	if(json_object_get_int(item) != 0)
	{
		switch(json_object_get_int(item))
		{
			case 1:
				res="身份证号码错误!";
				break;
			case 2:
				res="地区校验错误!";
				break;
			case 3:
				res="生日校验错误!";
				break;
			case 4:
				res="校验码校验错误!";
				break;
			default:
				res="未知错误，请联系brisk!";
		}

		json_object_put(item);
		json_object_put(obj);
		return error_data(res);
	}
	json_object_put(item);

	res=stradd(res,"性别:");
	item=json_object_object_get(obj,"sex");
	if(json_object_get_int(item) == 1)
		res=stradd(res,"男");
	else
		res=stradd(res,"女");
	json_object_put(item);

	object_stradd(&res," 生日:","birthday",obj);
	object_stradd(&res," 住址:","region",obj);

	json_object_put(obj);
	return res;
}

char *air_parse(char *str)
{
	json_object *obj;
	json_object *item;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(!obj) return error_data("查询出错，请联系brisk报告bug！");
	item=json_object_object_get(obj,"error");
	if(json_object_get_int(item) != 0)
	{
		json_object_put(item);
		item=json_object_object_get(obj,"errmsg");
		res=error_data(json_object_get_string(item));
		json_object_put(item);
		json_object_put(obj);

		return res;
	}
	json_object_put(item);

	object_stradd(&res,"城市:","city",obj);
	object_stradd(&res," 级别:","level",obj);
	object_stradd(&res," 更新时间:","update_time",obj);
	object_stradd(&res," AQI:","aqi",obj);
	object_stradd(&res," PM2.5/1h:","pm25",obj);
	object_stradd(&res," PM10/1h:","pm10",obj);
	object_stradd(&res," CO:","co",obj);
	object_stradd(&res," NO2:","no2",obj);
	object_stradd(&res," O3","o3",obj);
	object_stradd(&res," O3/8h","o3_8h",obj);
	object_stradd(&res," SO2","so2",obj);
	object_stradd(&res," 主要污染物:","primary_pollutant",obj);
	object_stradd(&res," 影响:","affect",obj);
	object_stradd(&res," 建议:","action",obj);

	json_object_put(obj);
	return res;
}

char *stack_parse(char *str)
{
	json_object *obj;
	json_object *item;
	json_object *temp;
	char *res=NULL;

	obj=json_tokener_parse(str);
	if(!obj)
		return error_data("查询出错了哦!");
	item=json_object_object_get(obj,"items");
	if(!item)
	{
		json_object_put(obj);
		return error_data("没有查询到结果!");
	}
	temp=json_object_array_get_idx(item,0);
	object_stradd(&res," ","title",temp);
	object_stradd(&res," <--> ","link",temp);

	json_object_put(temp);
	json_object_put(item);
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

char *website_error(int code)
{
	char *error;

	switch(code)
	{
		case -1:
			error="应用的key不存在或者key已经失效，请联系brisk!";
			break;
		case -2:
			error="无效的签名，请联系brisk!";
			break;
		case -3:
			error="无效的时间戳，请联系brisk!";
			break;
		case -4:
			error="超过当天查询次数限制!";
			break;
		case -5:
			error="查询过于频繁!";
			break;
		case -6:
			error="查询参数格式错误，例如某些API我们要求对参数进行base64编码，如果base64编码错误，服务器将返回此错误提示。请联系brisk！";
			break;
		case -7:
			error="缺少必需的查询参数。请联系brisk！";
			break;
		case -8:
			error="服务器繁忙";
			break;
		case -9:
			error="重复的时间戳，请联系brisk!";
			break;
		default:
			error="未知错误，请联系brisk！";
	}

	return error_data(error);
}
