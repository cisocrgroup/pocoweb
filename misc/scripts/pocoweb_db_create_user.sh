#!/bin/bash

set -e

if [ $# != 7 ]; then
    echo "Usage: $0 dbuser dbhost dbpass user-name user-email user-institute user-passwd"
    exit 1;
fi;

salt=$(openssl rand -base64 6)
passwd=$(openssl passwd -1 -salt $salt $7)

mysql --user $1 --host $2 --password=$3 <<EOF
use pocoweb;
insert into users (name, email, institute, passwd)
           values ("$4", "$5", "$6", "$passwd")
EOF
