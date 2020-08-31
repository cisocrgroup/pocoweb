#!/bin/bash

user=root@example.com
pass=password
host=localhost

while [[ $# -gt 0 ]]; do
    key=$1
    case $key in
	-u)
	    user=$2
	    shift
	    shift
	    ;;
	-p)
	    pass=$2
	    shift
	    shift
	    ;;
	-h)
	    host=$2
	    shift
	    shift
	    ;;
	*)
	    echo "Usage: $0 [-u user] [-p pass] [-h host]"
	    exit 1
    esac
done

# echo "$0 -u $user -p $pass -h $host"
url=https://$host/rest

# check version
version=$(curl -sk $url/api-version | jq -r '.version')
if [[ $version == "" ]]; then
    echo bad version
    exit 1
fi

# check authentification
data=$(echo {} | jq "{password:\"$pass\",email:\"$user\"}")
auth=$(curl -sk -d "$data" $url/login | jq -r '.auth')
if [[ $auth == "" ]]; then
    echo cannot authentificate
    exit 1
fi

# upload test data
zip=misc/data/test/1841-DieGrenzboten-abbyy-small.zip
pid=$(curl -sk -F "data=@$zip" -H 'Content-Type: application/zip' "$url/books?auth=$auth&author=testauthor&title=grenzboten&year=1841" | jq '.bookId')
if [[ $pid == "" ]]; then
    echo cannot upload $zip
    exit 1
fi

# check author
author=$(curl -sk "$url/books/$pid?auth=$auth" | jq -r '.author')
if [[ $author != "testauthor" ]]; then
   echo bad author: $author
   exit 1
fi

# check title
title=$(curl -sk "$url/books/$pid?auth=$auth" | jq -r '.title')
if [[ $title != "grenzboten" ]]; then
   echo bad title: $title
   exit 1
fi

# check year
year=$(curl -sk "$url/books/$pid?auth=$auth" | jq -r '.year')
if [[ $year != "1841" ]]; then
   echo bad year: $year
   exit 1
fi

# check first line
line=$(curl -sk "$url/books/$pid/pages/179392/lines/1?auth=$auth" | jq -r '.ocr')
if [[ $line != "Dmlschland md Belgien." ]]; then
   echo bad line: $line
   exit 1
fi

# remove book
code=$(curl -sk -X DELETE -w "%{http_code}" "$url/books/$pid?auth=$auth")
if [[ $code != "200" ]]; then
   echo cannot delete project: $pid
   exit 1
fi
