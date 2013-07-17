#!/bin/bash 

url=`exec/url_encode $1`

exec/regex \<a\ href=\"\/song\/.[^\"]* "`curl -s http://music.baidu.com/search?key=$url`" | sed 's/<a href=\"/http:\/\/music.baidu.com/g'

exit 0
