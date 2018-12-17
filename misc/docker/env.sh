#/bin/sh

# Base dirctory of volumes
#
# Set here or use export
# export PCW_BASE_DIR=/srv/pocoweb
# docker-compose ...
#PCW_BASE_DIR

# Settings for the mysql user
MYSQL_ROOT_PASSWORD=
MYSQL_USER=root
MYSQL_PASSWORD=
MYSQL_DATABASE=pocoweb
# Misc settings
PCW_DB_HOST=host.docker.internal
PCW_POCOWEB_HOST=http://pocoweb:8080
PCW_PCWAUTH_HOST=:8081
# Settings for the root user of pocoweb
PCW_ROOT_NAME=root
PCW_ROOT_EMAIL=root@example.com
PCW_ROOT_PASSWORD=password
PCW_ROOT_INSTITUTE=CIS
