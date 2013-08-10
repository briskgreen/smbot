#!/bin/bash 

key="your key"

in=`curl -s "https://www.googleapis.com/customsearch/v1?key=$key&cx=006431901905483214390:i3yxhoqkzo0&num=1&q=$1&searchType=image"`

link=`exec/regex \"link\":\ \".[^\"]* "$in" | sed 's/"link": "//g'`
des=`exec/regex \"snippet\":\ \".[^\"]* "$in" | sed 's/"snippet": "//g'`

echo "$des<--$link"

exit 0
