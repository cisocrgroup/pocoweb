#/bin/sh

sql=/etc/pcwd.sql
public_html=/apps/public_html

cp -r $public_html /www-data

ok=false
n=10
sleeps=2
while [[ $ok == false ]] && (($n > 0)); do
	n=$((n-1))
	sleep $sleeps
	mysql -h$PCW_DB_HOST -u$MYSQL_USER -p$MYSQL_PASSWORD $MYSQL_DATABASE < $sql \
		&& ok=true;
done
if [[ $ok == false ]]; then
   echo "cannot setup databse tables";
   exit 1;
fi
/usr/local/bin/pcwd $@ || exit 1
