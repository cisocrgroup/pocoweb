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

[![Build Status](https://travis-ci.org/cisocrgroup/pocoweb.svg?branch=doc)](https://travis-ci.org/cisocrgroup/pocoweb)

# Pocoweb
Pocoweb is a postcorrection tool for (historical) OCR data for the web.
It is based on [PoCoTo](https://github.com/cisocrgroup/PoCoTo).

If you encounter any problems, make sure to check the [misc notes]()
section on the bottom of this page.

## Docker
Pocoweb can be used and deployed using containers with [docker-compose](https://docs.docker.com/compose/).
Its main configuration can be found in the [docker-compose.yml](misc/docker/docker-compose.yml) file.

### Setup
There are five external directories, that the different container need to access.
Create them using `mkdir -p /srv/pocoweb/{www,project,db,tmp,language}-data`.
 * `/srv/pocoweb/www-data` contains all php, java-script and css files of the frontend
 * `/srv/pocoweb/project-data` is used to store the different OCR projects of Pocoweb
 * `/srv/pocoweb/db-data` contains the data of the mysql database
 * `/srv/pocoweb/language-data` contains the
   [language configurations](https://github.com/cisocrgroup/Resources/blob/master/manuals/profiler-manual.pdf)
   for the local profiler
 * `/srv/pocoweb/tmp-data` contains temporary data,
   that is used to upload project files to Pocoweb.
 * make sure that PHP can access the `srv/pocoweb/tmp-data` directory:
   `chmod a+w /srv/pocoweb/tmp-data`

It is possible to change the paths of the different directories.
Make sure to update the paths in the [docker-compose.yml](misc/docker/docker-compose.yml) and
Pocoweb [configuration](misc/docker/pocwoeb/pocoweb.conf) files accordingly.

The `/srv/pocoweb/www-data` directory contains the static files for Pocoweb's frontend.
You need to install the frontend to this directory using the command
`make PCW_FRONTEND_DIR=/srv/pocoweb/www-data install-frontend`,
from Pocoweb's root directory.

### Building the container images
Change into the `misc/docker` directory and build the containers using:
```
docker-compose build
```

You need to setup the tables of the databsase. After building the containers
-- before starting them -- start the database service and setup the tables:
```
docker-compose start db
mysql -h 127.0.0.1 -P 3306 -uuser-name -ppassword pocoweb < db/tables.sql
```

### Running the container
After building the containers in the `misc/docker` directory issue the following command:
```
docker-compose up
```

## Installation
The installation is a little bit involved. Various services have to be
configured and made to comunicate with each other.
In general you can allways take a look at some of the example
configuration files:
 * The Pocoweb [Dockerfile](misc/docker/pocoweb/Dockerfile)
 * The Pocoweb [configuration](misc/docker/pocoweb/pocoweb.conf)
 * The Nginx [configuration](misc/docker/nginx/nginx.conf)
 * The database [configuration](db/tables.sql)

### Pocoweb back-end
Pocoweb comes as a separate back-end process.
This daemon needs a running mysql database server.
The daemon is configured using the `config.ini` configuration file.
Its implementation can be found under the `rest/src` directory.

#### Dependencies
The back-end of Pocoweb is written in c++ and depends on the following (ubuntu)
libraries and tools (see also [the dependency listing](misc/docker/pocoweb/dependencies)):
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

#### Additional dependencies
Pocoweb manages some of its internal dependencies using git submodules.
It depends on the following git submodules (found in the `modules` directory):
* crow
* pugixml
* sqlpp11
* sqlpp11-connector-mysql
* date
* utfcpp

In order to load the internal dependencies, in the source directory of
Pocoweb issue the following command: `git submodule update --init`.
This will clone the submodules and download the internal dependencies of
Pocoweb. You must download all internal dependencies before you can
proceed to build Pocoweb.

#### Profiler
Pocoweb can optionally be used with a local (in respect to the rest-service)
profiler.
In order to use a local profiler, the profiler must be installed and
according language backends should have been created. Refer to
[the profiler manual](https://github.com/cisocrgroup/Resources/blob/master/manuals/profiler-manual.pdf)
for more information or read the Profilers [Dockerfile](misc/docker/profiler/Dockerfile).

http://localhost/project-data/udsvhhrbqzrswlcp/ocropus-book/0042/010001.bin.png

#### Compilation
After all dependencies have been installed (see above),
build the back-end using the command `make` (use `-jN` to speed up the
compilation, where `N` stands for the number of parallel build processes).

After the compilation has finished, you should test the back-end.
Execute Pocoweb's unit tests with `make test`.

#### Mysql
Pocoweb uses a mysql database to store its data.
You need to create a database called `pocoweb` with an according user account
and update Pocoweb's configuration file `config.ini`.

In order to create the tables for Pocoweb you can issue the command:
`mysql -h dbhost -u dbuser -p < db/tables.sql`.
You have to insert the password for the according mysql user.

#### User accounts
You have to create one user account for the configuration of Pocoweb.
All other user accounts can be created later from this account
(its also possible to create additional user accounts directly
in the database table `users`).
Change the according fields under the `[plugin-simple-login]` block
in the configuration file `config.ini`.

#### Starting Pocoweb
You can manually start the Pocoweb daemon calling `./pcwd config.ini`.
Pocoweb comes with a systemd unit file to handle Pocoweb with systemd.

Do not forget to copy the `pcwd` executable and the configuration file
to the appropriate places in your system (see the systemd service file).
Update and install the [unit file](misc/systemd/pococweb.service).
Enable the service using `systemd enable pocoweb.service`.
You can then manage Pocoweb using `systemd {restart,start,stop} pocoweb`.
Note, that you need to be root to execute all of the later commands.
It is possible to inspect Pocoweb's log using `journalctl [-f] -u pocoweb`.

### Pocoweb front-end
Pocoweb's front-end is written in PHP using some Javascript functions.
In order to run, an additional web server with support for PHP is needed.
Also the front-end relies on a running Pocoweb back-end process.
The front-end is implemented in the `frontend` directory.

In order to serve the front-end, you need to have a web server installed
on the host system. You have to enable PHP on the webserver and make sure
that the various limits on file uploads are set accordingly.

The web server must be able to redirect calls to the rest-service
to the Pocoweb back-end (see next section) and be able to serve paths
to the project directory of the back-end.

If you are unsure, take a look to the configuration files of the containers:
 * [nginx.conf](misc/docker/nginx/nginx.conf)
 * [pocoweb.conf](misc/docker/pocoweb/pocoweb.conf)

#### Daemon
The front-end needs Pocoweb's daemon to run.
You have to configure the daemon's endpoint in the `frontend/resources/config.php`
file setting the appropriate `$config['backend']['url']` variable.

If the daemon is running on `localhost:8080` for example,
you need to configure the redirection URL in your web server and insert
this URL in the configuration as well.

#### Installation
You need to install the front-end into your server's web-directory.
Since there are some files generated automatically (including this documentation)
it is not possible to merely copy the `frontend` directory.
If you change any configuration variables in `frontend/resources/config.php`,
make sure to reinstall the front-end.

To install the front-end to `/path/to/web/directory`,
issue the following command:
```
make install-frontend PCW_FRONTEND_DIR=/path/to/web/directory
```
This updates all needed files and installs them under the given directory.

## Folder structure
 * `rest/src` contains the back-end c++ implementation
 * `db` contains the database table definitions
 * `frontend` contains the web frontend files
 * `frontend/resources` contains the php templates and library functions
 * `frontend/public_html` contains images, html, php and javascript front-end files
 * `make` contains various helper makefiles
 * `modules` contains the git submodules

## Documentation
The basic usage and API Documentation is part of Pocoweb.
It can be found [here](frontend/public_html/doc.md).

## License

Attributed is free software, and may be redistributed under the terms specified in the [LICENSE] file.

[LICENSE]: /LICENSE

## Misc notes
Connect to the database:
 * make sure that the `ports` entry in the [docker-compose.yml](misc/docker/docker-compose.yml)
   publishes the database port: `"3306:3306"`
 * connect to the database with `mysql -h 127.0.0.1 -P 3306 -uuser -ppass pocoweb`

Settings for the file upload limits for PHP, nginx and Pocoweb:
 * Set `$config["frontend"]["upload"]["max_size"]` in [config.php](frontend/resources/config.php)
 * Set `client_max_body_size` (see [nginx.conf](misc/docker/nginx/nginx.conf))
 * Set `upload_max_file_size` and `post_max_size` (see [nginx.conf](misc/docker/nginx/nginx.conf),
   [upload.ini](misc/docker/fpm/upload.ini) and the [Dockerfile](misc/docker/fpm/Dockerfile))
