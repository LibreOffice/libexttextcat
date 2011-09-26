#!/bin/sh
cd ../langclass/LM
#take first guess
res=`cat ../ShortTexts/$1.txt | ../../src/testtextcat ../fpdb.conf | sed -e "s/].*/]/"`
if [ $res == "[$1--utf8]" ]; then
    exit 0
else
    exit 1
fi
