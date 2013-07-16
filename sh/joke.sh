#!/bin/bash 

url=`echo $1 | iconv -f utf-8 -t gbk`
word=`exec/url_encode $url`

exec/regex \<p\>.*\<\/p\> "`curl -s www.jijidi.com/plus/search.php?keyword=$word | iconv -f gbk`" | head -n 1 | tr -d "\'" | sed 's/[</*=*>,a-z]//g'

exit 0
