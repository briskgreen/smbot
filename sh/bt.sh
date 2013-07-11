#!/bin/bash 

URL='http://btdigg.org/search?info_hash=&q='

s=`curl -s -x 127.0.0.1:8087 $URL$1 | grep magnet:?xt=urn:btih: | awk -F"(=\")" '{print $9}' | awk -F"(magnet)" '{print $2}' | head -n 2 | awk -F"(\")" '{print $1}'`

echo magnet$s | sed 's/ //g'

exit 0
