#/bin/sh

config=/apps/pocoweb.conf
sql=/apps/tables.sql

user=$(git config -f $config --get db.user)
pass=$(git config -f $config --get db.pass)
host=$(git config -f $config --get db.host)
name=$(git config -f $config --get db.name)

mysql -h$host -u$user -p$pass $name < $sql || exit 1
/apps/pocoweb $config || exit 1
