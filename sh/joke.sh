#!/bin/bash 

url=`exec/url_encode $1`

exec/regex \<p\>.[^\\\s]*\<\/p\> "`curl -s "http://www.fangtang8.com/?s=$1"`" | sed 's/<\/p>//g' | sed 's/<p>//g' | sed 's/<br\ \/>//g' | tr -d '\n'

exit 0
