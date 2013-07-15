#!/bin/bash 

curl -s "http://qq.ip138.com/idsearch/index.asp?action=idcard&userid=$1&B1=%B2%E9+%D1%AF" | iconv -f gbk | grep '出生日期' | sed 's/[a-z,A-Z,<>\/=\"&;]//g' | sed 's/ [0-9]//g'

exit 0
