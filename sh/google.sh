#!/bin/bash 

key="your key"
in=`curl -s "https://www.googleapis.com/customsearch/v1?key=$key&cx=006431901905483214390:i3yxhoqkzo0&num=1&q=$1"`
link=`exec/regex link\":\ \"*http.[^,]* "$in" | sed 's/link\": \"//g' | sed 's/\"/ <--/g'`
des=`exec/regex snippet\":.*\"htmlSnippet\": "$in" | sed 's/snippet": //g' | sed 's/"htmlSnippet"://g'`

echo $link$des

exit 0
