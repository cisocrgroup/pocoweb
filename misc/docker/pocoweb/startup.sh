#/bin/sh

config=/apps/pocoweb.conf
sql=/apps/tables.sql
public_html=/apps/public_html
resoures=/apps/resources

user=$(git config -f $config --get db.user)
pass=$(git config -f $config --get db.pass)
host=$(git config -f $config --get db.host)
name=$(git config -f $config --get db.name)

cp -vr $public_html /www-data
cp -vr $resoures /www-data
sleep 10
mysql -h$host -u$user -p$pass $name < $sql || exit 1
/apps/pocoweb $config || exit 1
