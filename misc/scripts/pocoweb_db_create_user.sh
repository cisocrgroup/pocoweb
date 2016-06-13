#!/usr/bin/bash

set -e

if [ $# != 5 ]; then
    echo "Usage: $0 dbuser user-name user-email user-institute user-passwd"
    exit 1;
fi;

salt=$(openssl rand -base64 6)
passwd=$(openssl passwd -1 -salt $salt $5)

mysql --user $1 <<EOF
use pocoweb;
insert into users (name, email, institute, passwd)
           values ("$2", "$3", "$4", "$passwd")
EOF
