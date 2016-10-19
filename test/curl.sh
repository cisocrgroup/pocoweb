#!/bin/bash
#set -x

#usage: $0 cookie-file expected-response-code args...
cookie=$1
shift
code=$1
shift

res=$(curl -s -o /dev/null -b $cookie -c $cookie -w '%{http_code}' $@) || exit

if [ ! $code = $res ]; then
	echo "[error] expected $code but got $res" 1>&2
	exit 1
fi
