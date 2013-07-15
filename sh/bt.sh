#!/bin/bash 

exec/regex magnet\:\?xt=urn\:btih\:*.[^\"]* "`curl -s -x 127.0.0.1:8087 "http://btdigg.org/search?info_hash=&q=$1"`"

exit 0
