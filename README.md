```
..............................................
..... ______...._______...___...___.... ___...
.... /     /.../  ____/../  /../  /... /  /...
... /  // /___/  /._____/  /../  /____/  /___.
.../  ___/   /  /./    /  / \/  /    /      /.
../  / / // /  /_/_// /  /  /  / //_/   // /..
./__/ /____/______/__/________/_____\_____/...
..............................................
```

# Dependencies
 * g++-5
 * libbost-dev
 * make
 * libssl-dev
 * libmariadbclient (mariadb is not needed)

# Folder structure
 * `rest/src` contains the back-end c++ implementation
 * `db` contains the database table definitions
 * `frontend` contains the web frontend files
 * `frontend/resources` contains the php templates and library functions
 * `frontend/public_html` contains images, html, php and javascript front-end files
 * `make` contains various helper makefiles
 * `modules` contains the git submodules


# Misc
```$ apt-get install g++-5 libbost-dev libmysqlcppconn-dev make libssl-dev```

## Settings for file upload

### Nginx
 * set `client_max_body_size 500M;` in `/etc/nginx/nginx.conf`
 * `client_max_body_size` can be set in the contexts: `http`, `server`, `location`
 * add the following to `/etc/nginx/sites-available/default`:
```
location~ \.php$ {
	# ...
	fastcgi_param PHP_VALUE "upload_max_filesize=500M\npost_max_size=500M";
}
```

### PHP
 * set `file_uploads = On` in `php.ini`
 * set `upload_max_file_size = 500M` in `php.ini`
