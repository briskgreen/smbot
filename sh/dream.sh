#!/bin/bash 

url=`echo $1 | iconv -f utf-8 -t gbk`
word=`exec/url_encode $url`

exec/regex \<dd\>.*\</dd\> "`curl -s "www.zgjm.org/plus/search.php?q=$word&Submit=%D6%DC%B9%AB%BD%E2%C3%CE" | iconv -f gbk`" | tr -d "</dd>\'" | sed 's/[a-z,=]//g' | head -n 1

exit 0
