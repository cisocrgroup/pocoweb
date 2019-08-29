make -C ../ INSTALL_DIR=/tmp/pocoweb install || exit 1
scp -r /tmp/pocoweb/public_html/ tobias@pocoweb:/srv/pocoweb/www-data/ || exit 1
#rsync -av /tmp/pocoweb/public_html/ tobias@pocoweb:/srv/pocoweb/www-data/ || exit 1
#cp -r /Users/tobiasenglmeier/pocoweb/frontend/public_html/ /Users/tobiasenglmeier/pocoweb_data/www-data/public_html/
