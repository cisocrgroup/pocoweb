#

# user, host and password for the database
PCW_DB_USER="user"
PCW_DB_HOST="localhost"
PCW_DB_PASS="password"

# settings for pocowebd
PCW_DAEMON_HOST="localhost"
PCW_DAEMON_PORT=8080
PCW_DAEMON_THREADS=4
PCW_LOG_FILE="pocoweb.log"

# default users
# each entry: name,email,institute,passwd
PCW_DEF_USERS += "flo,fake-email@example.org,CIS,flo"
PCW_DEF_USERS += "uwe,fake-email@example.org,CIS,uwe"
PCW_DEF_USERS += "klaus,fake-email@example.org,CIS,klaus"
