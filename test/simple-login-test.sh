#!/bin/sh
set -x

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
host=$(git config -f $config --get daemon.host)
port=$(git config -f $config --get daemon.port)
rest/pcwd $config 2>/dev/null &
pid=$!
echo $pid > $tdir/pid

# -XGET is automatically inferred
curl -c $cookies -v [$host]:$port/api-version
curl -c $cookies -v [$host]:$port/login/user/$user/pass/$pass
#curl -XPOST -c /tmp/cookies.txt -v $host:$port/login/username/$name/password/$pass

