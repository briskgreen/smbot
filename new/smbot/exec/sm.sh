#!/bin/bash 

res=`curl -s "http://xiaofengrobot.sinaapp.com/api.php?text=$1" | tr -d "\r"`

echo $res

exit 0
