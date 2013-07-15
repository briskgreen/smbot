#!/bin/bash 

exec/regex /questions.*title.[^\>]* "`curl -s -x 127.0.0.1:8087 http://stackoverflow.com/search?q=$1 | grep Q: | head -n 1`" | sed 's/" title="/ -->/g' | sed 's/&quot;/"/g'

exit 0
