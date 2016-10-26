#!/bin/bash
#set -x
set -e

verbose=""
tdir=$(mktemp -d)

function finish {
	if [ -f $tdir/pid ]; then
		kill -9 $(cat $tdir/pid)
	fi
	rm -rf $tdir
}
trap finish EXIT

config=$tdir/config.ini
cookies=$tdir/cookies.txt
cp misc/default/config.def.ini $config

host=$(git config -f $config --get daemon.host) 
port=$(git config -f $config --get daemon.port) 
user=$(git config -f $config --get plugin-simple-login.default-user) 
pass=$(git config -f $config --get plugin-simple-login.default-pass) 

# start server if not called with no-start-server
if [ $1 != "no-start-server" ]; then
	rest/pcwd $config 2>/dev/null &
	pid=$!
	echo $pid > $tdir/pid
	sleep 1
fi;

# -XGET is automatically inferred
echo "/api-version?" 1>&2
test/curl.sh $tdir 200 [$host]:$port/api-version
echo "/login/user/pocoweb?" 1>&2
test/curl.sh $tdir 200 [$host]:$port/login/user/$user/pass/$pass
echo "/logged-in?" 1>&2
test/curl.sh $tdir 200 [$host]:$port/logged-in
echo "/create/user/test?" 1>&2
test/curl.sh $tdir 201 [$host]:$port/create/user/test/pass/test123
echo "/login/user/test?" 1>&2
test/curl.sh $tdir 200 [$host]:$port/login/user/test/pass/test123
echo "/logged-in?" 1>&2
test/curl.sh $tdir 200 [$host]:$port/logged-in
echo "/update/user/test/set/email?" 1>&2
test/curl.sh $tdir 200 [$host]:$port/update/user/test/set/email/foo@bar
echo "/update/user/test/set/institute?" 1>&2
test/curl.sh $tdir 200 [$host]:$port/update/user/test/set/institute/test
echo "delete/user/test?" 1>&2
test/curl.sh $tdir 200 [$host]:$port/delete/user/test
