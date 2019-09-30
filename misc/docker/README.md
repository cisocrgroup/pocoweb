# Docker compose
## Setup
- set PCW_BASE_DIR variable in `.env` file
- edit `externalURL` variable in `frontend/public_html/api/config.php`
- edit `externalURL` variable in `frontend/resources/config.php`
- update settings in env.sh file.
- `mkdir -p $PCW_BASE_DIR/tmp-data`
- `chmod a+rwx $PCW_BASE_DIR/tmp-data`
- PCW_BASE_DIR=/base/dir docker-compose build
- PCW_BASE_DIR=/base/dir docker-compose up (starts in forground) or
- PCW_BASE_DIR=/base/dir docker-compose run (starts in background)
- setup htpasswd file: `echo 'password' | htpasswd -ci
  /srv/pocoweb/www-data/public_html htpasswd user`

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
  - get info about mariadb container: `sudo docker inspect $(sudo
    docker ps | awk -e '/mariadb/{print $1}')`
## Frontend development
- Update frontend data:
`make -j V="" PCW_FRONTEND_DIR=$PCW_BASE_DIR/www-data install-frontend`

## https via letsencrypt
 - use `--dry-run` for testing
 - Create a new certificate: `sudo PCW_BASE_DIR=/srv/pocoweb
   docker-compose run --rm letsencrypt certbot certonly --webroot
   --email finkf@cis.lmu.de --agree-tos -w /var/www/letsencrypt -d
   pocoweb.cis.lmu.de --force-renewal`
 - Renew a certificate: `sudo PCW_BASE_DIR=/srv/pocoweb docker-compose
   run --rm letsencrypt certbot renew`
 - Restart nginx with new certificate: `sudo PCW_BASE_DIR=/srv/pocoweb
   docker-compose kill -s SIGHUB nginx`
