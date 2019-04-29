# Setup
- edit `externalURL` variable in `frontend/public_html/api/config.php`
- edit `externalURL` variable in `frontend/resources/config.php`
- update settings in env.sh file.
- `mkdir -p $PCW_BASE_DIR/tmp-data`
- `chmod a+rwx $PCW_BASE_DIR/tmp-data`
- PCW_BASE_DIR=/base/dir docker-compose build
- PCW_BASE_DIR=/base/dir docker-compose up (starts in forground) or
- PCW_BASE_DIR=/base/dir docker-compose run (starts in background)

# Frontend development
- Update frontend data:
`make -j V="" PCW_FRONTEND_DIR=$PCW_BASE_DIR/www-data install-frontend`
