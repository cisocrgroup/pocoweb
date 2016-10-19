#!/bin/sh
#set -x

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

# start server
host=$(git config -f $config --get daemon.host) || exit 1
port=$(git config -f $config --get daemon.port) || exit 1
user=$(git config -f $config --get plugin-simple-login.default-user) || exit 1
pass=$(git config -f $config --get plugin-simple-login.default-pass) || exit 1

rest/pcwd $config 2>/dev/null &
pid=$!
echo $pid > $tdir/pid
sleep 1

# -XGET is automatically inferred
curl -s -o /dev/null -w '%{http_code}\n' [$host]:$port/api-version
curl -s -o /dev/null -c $cookies $verbose -w '%{http_code}\n' [$host]:$port/login/user/$user/pass/$pass
curl -s -o /dev/null -b $cookies $verbose -w '%{http_code}\n' [$host]:$port/logged-in
curl -s -o /dev/null -b $cookies $verbose -w '%{http_code}\n' [$host]:$port/create-user/user/test/pass/test123
#curl -XPOST -c /tmp/cookies.txt -v $host:$port/login/username/$name/password/$pass

