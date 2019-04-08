#/bin/sh

config=/apps/pocoweb.conf
sql=/apps/tables.sql
public_html=/apps/public_html
resoures=/apps/resources

cp -vr $public_html /www-data
cp -vr $resoures /www-data
sleep 10

mysql -h$PCW_DB_HOST -u$MYSQL_USER -p$MYSQL_PASSWORD $MYSQL_DATABASE < $sql || exit 1
/apps/pocoweb $config || exit 1
