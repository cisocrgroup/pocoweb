# Setup
- update settings in env.sh file.
- PCW_BASE_DIR=/base/dir docker-compose build
- PCW_BASE_DIR=/base/dir docker-compose up (starts in forground) or
- PCW_BASE_DIR=/base/dir docker-compose run (starts in background)

# Frontend development
- Update frontend data:
`make -j V="" PCW_FRONTEND_DIR=$PCW_BASE_DIR/www-data install-frontend`
