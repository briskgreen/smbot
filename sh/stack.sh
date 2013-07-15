#!/bin/bash 

exec/regex /questions.*title.[^\>]* "`curl -s http://stackoverflow.com/search?q=$1 | grep Q: | head -n 1`" | sed 's/" title="/ -->/g' | sed 's/&quot;/"/g'

exit 0
