#!/bin/sh
if [ $# != 1 ]; then
	echo "usage $0 env";
	exit 1;
fi

source $1
cat <<EOF
[Unit]
Description=${PCW_SYSTEMD_DESCRIPTION}
Documentation=${PCW_SYSTEMD_DOCUMENTATION}
Requires=${PCW_SYSTEMD_REQUIRES}

[Service]
Type=${PCW_SYSTEMD_TYPE}
PIDFile=${PCW_DAEMON_PIDFILE}
ExecStart=${PCW_POCOWEB_EXE} ${PCW_POCOWEB_CONFIG}
Restart=${PCW_SYSTEMD_RESTART}

[Install]
WantedBy=${PCW_SYSTEMD_WANTEDBY}
EOF
