#!/bin/bash 

url=`exec/url_encode $1`

exec/regex \<p\>.[^\<]* "`curl -s "http://www.fangtang8.com/?s=$url"`" | sed 's/<p>//g'

exit 0
