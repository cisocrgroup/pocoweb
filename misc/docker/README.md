# Setup
- edit `externalURL` variable in `frontend/public_html/api/config.php`
- edit `externalURL` variable in `frontend/resources/config.php`
- update settings in env.sh file.
- `mkdir -p $PCW_BASE_DIR/tmp-data`
- `chmod a+rwx $PCW_BASE_DIR/tmp-data`
- PCW_BASE_DIR=/base/dir docker-compose build
- PCW_BASE_DIR=/base/dir docker-compose up (starts in forground) or
- PCW_BASE_DIR=/base/dir docker-compose run (starts in background)

## Database
- connect to the database requires the following steps:
  - find the container's ID of the mariadb container using `docker ps`
    or use the container's default name `docker_db_1`
  - find the container's IP address using `docker inspect --format
    '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}'`
  - `docker inspect $(docker ps | grep mariadb | awk -e '{print $1}')`
    prints all information about the database docker container
  - from the server connect to the database container with the
    according ip address `mysql -h 172.18.0.2 -u pocoweb -p pocoweb`
# Frontend development
- Update frontend data:
`make -j V="" PCW_FRONTEND_DIR=$PCW_BASE_DIR/www-data install-frontend`
