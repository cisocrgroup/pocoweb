#/bin/sh

# MYSQL DATA
MYSQL_ROOT_PASSWORD=example
MYSQL_USER=pocoweb
MYSQL_PASSWORD=pocoweb1998
MYSQL_DATABASE=pocoweb

PCW_DB_HOST=http://db:3306
PCW_POCOWEB_HOST=http://pocoweb:8080
PCW_PCWAUTH_HOST=:8081

    # environment:
    #   - PCWAUTH_DB=pocoweb:pocoweb1998@(db)/pocoweb
    #   - PCWAUTH_POCOWEB=http://pocoweb:8080
    #   - PCWAUTH_HOST=:8081
