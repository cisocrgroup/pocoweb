#!/bin/bash

set -e

if [ $# -ne 2 ];
	then echo "Usage $0 <config> <tmp-dir>"
	exit 1;
fi
config=$1
tdir=$2
host=$(git config -f $config --get daemon.host) 
port=$(git config -f $config --get daemon.port) 
user=$(git config -f $config --get plugin-simple-login.default-user) 
pass=$(git config -f $config --get plugin-simple-login.default-pass) 

test/curl.sh $tdir 200 [$host]:login/user/$user/pass/$pass
