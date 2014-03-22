#include "parse.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *error_data(char *msg);
char *stradd(char *from,char *sub);

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
