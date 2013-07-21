#!/bin/bash 

url=`exec/url_encode $1`

exec/regex \<div\ class=\"sb_tlst\ sbt_ad\"\>.*href=\".*target=\"_blank\".*\<cite\> "`curl -s "http://cn.bing.com/search?q=$url"`" | awk -F"(<cite>)" '{print $1}' | awk -F"(</p>)" '{print $1}' | sed 's/<div class="sb_tlst sbt_ad">.[^<]*<a href="//g' | sed 's/" target="_blank".*<p>/ <--/g' | sed 's/<strong>//g' | sed 's/<\/strong>//g' | sed 's/<div class="sb_tlst sbt_ad"><a href="//g' | sed 's/" class="sb_fav//g' | sed 's/<.*>//g'

exit 0
