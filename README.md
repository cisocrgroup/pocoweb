```
..............................................
..... ______....______....___...___.... ___...
.... /     /.../  ___/..../  /../  /... /  /..
... /  // /___/  /.._____/  /../  /____/  /_..
.../  ___/   /  /../    /  / \/  /    /     \.
../  / / // /  /__/ // /  /  /  / //_/   // /.
./__/ /____/\____/____/________/_____\_____/..
..............................................
```

[![Build Status](https://travis-ci.org/cisocrgroup/pocoweb.svg?branch=doc)](https://travis-ci.org/cisocrgroup/pocoweb)

# Pocoweb
Pocoweb is a postcorrection tool for (historical) OCR data for the web.
It is based on [PoCoTo](https://github.com/cisocrgroup/PoCoTo).

If you encounter any problems, make sure to check the misc notes
section on the bottom of this page.

## Deployment
Pocoweb should be deployed using
[docker-compose](https://docs.docker.com/compose/).  The main
configuration can be found in the
[docker-compose.yml](docker-compose.yml) file.

In theory it should be possible to deploy Pocoweb on any platform that
supports docker.  But there apears to be a problem with the mariadb
docker image on Windows.

For the remainder of the section, it is assumed that the Pocoweb
source directory is checked out at some location.  All examples are
meant to be run from the project's source directory.

### Setup
#### Default directories
Pocoweb uses a base directory to store its data (database tables, line
images, etc.).  It defaults to `/srv/pocoweb`, but one can change the
location of the base directory.  In this case change all instances of
`/srv/pocoweb` to the desired location for Pocoweb's base directory.

Create all required directories using `mkdir -p
/srv/pocoweb{ww,project,db,tmp,language}-data`.

#### Configuration file
Copy the default configuration file to the source directory using `cp
misc/config/env.sh ./` and adjust it accordingly.  In the first block
you can configure the access parameters to the internal database of
Pocoweb.  The database is not accessible from outside the host
machine, but you should at least customize the database's password.

The second block in the configuration file contains settings for the
root administrator account.  This account is created at the first
start of Pocoweb and must be used to access Pocoweb the first time.
You should change all the settings accordingly.

If you really want to use Pocoweb's default settings you can skip the
edition of the configuration file and just use the default one (in
this case you do not even have to copy it).

#### Certificate
Pocoweb uses https so according certificate files are needed.  If you
have some, copy the key file to `services/nginx/key.pem` and the cert
file to `services/nginx/cert.pem`.

You can create a self signed certificate using e.g.
[openssl](https://www.openssl.org/) or any other tool to generate an
according certificate. Using openssl just execute `openssl req -x509
-nodes -days 365 -newkey rsa:2048 -keyout services/nginx/key.pem -out
services/nginx/cert.pem` from the root directory to generate it.  You
can skip this step, since the build process will generate a self
signed certificate using openssl automatically if no certificate was
provided.

#### Last steps
Last but not least checkout some external
dependencies using `git submodule update --init`.

### Starting pocoweb
After having gone through all the above steps, one can build and start
pocoweb using the provided Makefile.  If no configuration file is
provided the build process will use the [default configuration
file](misc/config/env.sh). If no certificate is provided the build
process will automatically generate one.  You will be prompted some
initial configuration in this case.

To build and start Pocoweb the following tools have to be installed:
* `docker`
* `docker-compose`
* `make`
* `openssl` (if no certificate is provided)

To deploy Pocoweb just execute `make docker-start`.  This command will
build all required docker images and start up the docker service.
Depending on your configuration you might need to used an
administrator account for you machine or use `sudo`.

To stop the service execute `make docker-stop`. If you want to inspect
the log files execute `docker-compose logs -f` (you can use CTRL-c to
exit the log).

If you did no use the default base directory `/srv/pocoweb` you have
to set the according path in the call to make: `make
PCW_BASE_DIR=/my/custom/path docker-start`.

If you encouter any issues deploying Pocoweb feel free to open an
issue on [github](https://github.com/issues).

### Disable https
It is possible to use pocoweb without https.  You should not do it,
since the pocoweb frontend has to send emails, password and
authentification tokens in order to do its work.  If you still want to
disable https for pocoweb, you have to edit the nginx configuration
and Dockerfile manually and then start the server normally (see
above).

[Nginx's configuration file](services/nginx/nginx.conf):
```
user nginx;
worker_processes auto;
pid /run/nginx.pid;

events {
	worker_connections 768;
	# multi_accept on;
}

http {
	##
	# Basic Settings
	##
	sendfile on;
	tcp_nopush on;
	tcp_nodelay on;
	keepalive_timeout 65;
	types_hash_max_size 2048;
	server_tokens off;

	# server_names_hash_bucket_size 64;
	# server_name_in_redirect off;
	include /etc/nginx/mime.types;
	default_type application/octet-stream;

	##
	# SSL Settings
	##
	ssl_protocols TLSv1 TLSv1.1 TLSv1.2; # Dropping SSLv3, ref: POODLE
	ssl_prefer_server_ciphers on;

	##
	# Logging Settings
	##
	access_log /var/log/nginx/access.log;
	error_log /var/log/nginx/error.log;

	##
	# Gzip Settings
	##
	gzip on;
	gzip_disable "msie6";

    server { # redirect http -> https
        listen 80;
        server_name pocoweb.cis.lmu.de;
	    client_max_body_size 2000M;
	    root /www-data/public_html;
	    index index.html;

	    location / {
	        try_files $uri $uri/ =404;
	    }
	    location /rest/ {
	        rewrite            ^/rest/(.*)$ /$1 break;
	        proxy_pass         http://pcwauth;
	        proxy_redirect     off;
	        proxy_set_header   Host $host;
	        proxy_set_header   X-Real-IP $remote_addr;
	        proxy_set_header   X-Forwarded-For $proxy_add_x_forwarded_for;
	        proxy_set_header   X-Forwarded-Host $server_name;
	    }
	    location /project-data {
	        alias /project-data;
	    }
	}
}
```

[Nginx's Dockerfile](services/nginx/Dockerfile):
```Dockerfile
FROM nginx:alpine
MAINTAINER Florian Fink <fink@cis.lmu.de>

VOLUME /www-data /project-data
COPY nginx.conf /etc/nginx/nginx.conf

EXPOSE 80
EXPOSE 443
CMD ["nginx-debug", "-g", "daemon off;"]
```

## Documentation
The basic usage and API Documentation is part of Pocoweb.  It can be
found [here](frontend/public_html/doc.md).

## Development
### Internal dependencies
The back-end of Pocoweb is written in c++ and depends on the following
(ubuntu) libraries and tools (see also [the dependency
listing](misc/docker/pocoweb/dependencies)):
* libboost-all-dev
* make
* g++
* git
* cmake
* libmysqlclient-dev
* mysql-client
* libleptonica-dev
* libssl-dev
* libcurl4-gnutls-dev
* python-pyparsing

If you are on ubuntu, you can do
`sudo apt-get install $(cat misc/docker/pocoweb/dependencies)`
to automatically install all dependencies.

### Additional dependencies
Pocoweb manages some of its internal dependencies using git
submodules.  It depends on the following git submodules (found in the
`modules` directory):
* crow
* pugixml
* sqlpp11
* sqlpp11-connector-mysql
* date
* utfcpp

In order to load the internal dependencies, in the source directory of
Pocoweb issue the following command: `git submodule update --init`.
This will clone the submodules and download the internal dependencies
of Pocoweb. You must download all internal dependencies before you can
proceed to build Pocoweb.

### Compilation
After all dependencies have been installed (see above),
build the back-end using the command `make` (use `-jN` to speed up the
compilation, where `N` stands for the number of parallel build processes).

After the compilation has finished, you should test the back-end.
Execute Pocoweb's unit tests with `make test`.

### Mysql
Pocoweb uses a mysql database to store its data.  You need to create a
database called `pocoweb` with an according user account and use the
according settings to start pocoweb.

In order to create the tables for Pocoweb you can issue the command:
`mysql -h dbhost -u dbuser -p < db/tables.sql`.
You have to insert the password for your mysql user.

### Pocoweb front-end
Pocoweb's front-end is written in Javascript.  They reside under
`/srv/pocoweb/www-data/`. To update them use `make -C frontend
install`.  If you use a custom path use `make -C frontend
INSTALL_DIR=my/custom/path/to/www-data install`.

## Folder structure
 * `rest/src` contains the back-end c++ implementation
 * `db` contains the database table definitions
 * `frontend` contains the web frontend files
 * `frontend/public_html` contains images, html, javascript front-end
   files
 * `make` contains various helper makefiles
 * `modules` contains the git submodules
 * `services` contains helper services (profiler, http-server, ...)
   need to run pocoweb

## Misc notes
* connect to the database requires the following steps:
  * find the container's ID of the mariadb container using `docker ps`
    or use the container's default name `docker_db_1`
  * find the container's IP address using `docker inspect --format
    '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}'`
  * `docker inspect $(docker ps | grep mariadb | awk -e '{print $1}')`
    prints all information about the database docker container
  * from the server connect to the database container with the
    according ip address `mysql -h 172.18.0.2 -u pocoweb -p pocoweb`
  * get info about mariadb container: `sudo docker inspect $(sudo
    docker ps | awk -e '/mariadb/{print $1}')`

## License

Attributed is free software, and may be redistributed under the terms
specified in the [LICENSE] file.

[LICENSE]: /LICENSE
