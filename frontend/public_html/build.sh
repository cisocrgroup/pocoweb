make -C ../../ PCW_FRONTEND_DIR=/tmp/pocoweb install-frontend
scp -r /tmp/pocoweb/public_html/ tobias@pocoweb:/srv/pocoweb/www-data/

#cp -r /Users/tobiasenglmeier/pocoweb/frontend/public_html/ /Users/tobiasenglmeier/pocoweb_data/www-data/public_html/
