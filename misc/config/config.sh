#!/bin/bash
if [ $# != 1 ]; then
	echo "usage $0 env";
	exit 1;
fi

source $1
cat <<EOF
[db]
host = ${PCW_DB_HOST}
user = ${PCW_DB_USER}
pass = ${PCW_DB_PASS}
db = ${PCW_DB_NAME}
debug = ${PCW_DB_DEBUG}
connections = ${PCW_DB_CONN}

[daemon]
host = ${PCW_DAMON_HOST}
port = ${PCW_DAEMON_PORT}
# threads = 0 means to using std::thread::hardware_concurrency
threads = ${PCW_DAEMON_THREADS}
detach = ${PCW_DAEMON_DETACH}
user = ${PCW_DAEMON_USER}
sessions = ${PCW_DAEMON_SESSIONS}
basedir = ${PCW_DAEMON_BASEDIR}
projectdir = ${PCW_DAEMON_PROJECTDIR}
pidfile = ${PCW_DAEMON_PIDFILE}

# if daemon.detach = true, log uses syslog; else it uses stderr.
[log]
# available levels: debug,info,warning,error,critical; default is info
level = ${PCW_LOG_LEVEL}

[profiler]
exe = ${PCW_PROFILER_EXE}
langdir = ${PCW_PROFILER_LANGDIR}
jobs = ${PCW_PROFILER_JOBS}
minweight = ${PCW_PROFILER_MINWEIGHT}
debug = ${PCW_PROFILER_DEBUG}

##
## plugins to load
## each plugin section must start with 'plugin-'
##
[plugin-simple-login]
path = ${PCW_LOGIN_PATH}
default-user = ${PCW_LOGIN_USER}
default-pass = ${PCW_LOGIN_PASS}
default-email = ${PCW_LOGIN_EMAIL}
default-institute = ${PCW_LOGIN_INSTITUTE}
EOF
