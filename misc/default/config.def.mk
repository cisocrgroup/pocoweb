#

# user, host and password for the database
PCW_DB_USER="user"
PCW_DB_HOST="localhost"
PCW_DB_PASS="password"

# settings for pocowebd
PCW_DAEMON_HOST="localhost"
PCW_DAEMON_PORT=8080
PCW_DAEMON_THREADS=4
PCW_DAEMON_USER="pocoweb"
PCW_DAEMON_SESSIONS=100
PCW_LOG_FILE="pocoweb.log"

# default users
# each entry: name,email,institute,passwd
PCW_DEF_USERS += "pocoweb,pocoweb@example.org,CIS,pocoweb"
PCW_DEF_USERS += "flo,email1@example.org,CIS,flo"
PCW_DEF_USERS += "uwe,email2@example.org,CIS,uwe"
PCW_DEF_USERS += "klaus,email3@example.org,CIS,klaus"

# default books
# each entry: type,owner,author,title,year,desc,uri,path[,users]*
PCW_DEF_BOOKS += "hocr,pocoweb,Hobbes,Leviathan,1668,example-document,uri,../misc/ocr/hobbes,flo,uwe,klaus"
