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
 * libmysqlcppconn-dev
 * make
 * libssl-dev

# Misc
```$ apt-get install g++-5 libbost-dev libmysqlcppconn-dev make libssl-dev```

## Settings for file upload

### Nginx
 * set `client_max_body_size 2000M;` in `/etc/nginx/nginx.conf`
 * `client_max_body_size` can be set in the contexts: `http`, `server`, `location`

### PHP
 * set `file_uploads = On` in `php.ini`
 * set `upload_max_file_size = 2000M` in `php.ini`
