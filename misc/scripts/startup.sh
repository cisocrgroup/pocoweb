#/bin/sh

sql=/etc/pcwd.sql
ok=false
n=10
sleeps=2
# Wait until database is read (10 tries, sleep 2s between each try).
while [[ $ok == false ]] && (($n > 0)); do
	n=$((n-1))
	sleep $sleeps
	mysql -hdb -u$MYSQL_USER -p$MYSQL_PASSWORD $MYSQL_DATABASE < $sql \
		&& ok=true;
done
if [[ $ok == false ]]; then
   echo "cannot setup databse tables";
   exit 1;
fi
/usr/local/bin/pcwd $@ || exit 1
