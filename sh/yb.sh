#!/bin/bash 

#curr=`pwd`
addr=`echo $1 | iconv -f utf-8 -t gbk`
url=`exec/url_encode $addr`

exec/regex [0-9]\\{6\\}[^\<]* "`curl -s "http://opendata.baidu.com/post/s?wd=$url&p=mini&rn=20" | grep begin`"

exit 0
