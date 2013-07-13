#!/bin/bash 

exec/regex itle:.[^\/\']* "`curl -s http://php.weather.sina.com.cn/search.php?city=$1\&dpc=1 | iconv -f gbk | grep title:\'`" | awk -F"(:)" '{print $2}'

exit 0
