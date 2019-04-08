#!/bin/bash
#set -x
set -e
#usage: $0 tmpdir expected-response-code args...

cookie=$1/cookies.txt
output=$1/output.txt
shift
code=$1
shift

res=$(curl -o $output -s -b $cookie -c $cookie -w '%{http_code}' $@) || exit

if [ $code = $res ]; then
	echo $(cat $output)
else
	echo "[error] expected $code but got $res" 1>&2
	exit 1
fi
