#/bin/sh

# Base dirctory of volumes
#
# Set here or use export if you want another value:
# export PCW_BASE_DIR=/srv/pocoweb
# docker-compose ...
# PCW_BASE_DIR=$(PCW_BASE_DIR:-/srv/pocoweb)
PCW_BASE_DIR=/srv/pocoweb

# Settings for the mysql user
MYSQL_ROOT_PASSWORD=example
MYSQL_USER=pocoweb
MYSQL_PASSWORD=pocoweb1998
MYSQL_DATABASE=pocoweb

# default root user (created at first startup)
PCW_ROOT_NAME=root
PCW_ROOT_EMAIL=root@example.com
PCW_ROOT_PASSWORD=password
PCW_ROOT_INSTITUTE=CIS
