#!/bin/bash 

in=`curl -s "https://www.googleapis.com/customsearch/v1?key=yourkey&cx=006431901905483214390:i3yxhoqkzo0&num=1&alt=atom&q=$1"`
url=`exec/regex href=\".[^\"]* "$in" | sed 's/href="//g'`
des=`exec/regex \<summary.*\<\/summary\> "$in" | sed 's/<sum.[^;]*//g' | sed 's/.b&.[^;]*//g' | sed 's/&.[^;]*//g' | sed 's/;//g' | sed 's/<\/summary>//g' | sed 's/br//g' | sed 's/nbsp//g'`

echo "$url <--$des"

exit 0
