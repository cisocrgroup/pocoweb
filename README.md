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
# Pocoweb
Pocoweb is a postcorrection tool for (historical) OCR data for the web.
It is based on [PoCoTo](https://github.com/cisocrgroup/PoCoTo).

## Installation
### Pocoweb back-end
Pocoweb comes as a separate back-end process.
This daemon needs a running mysql database server.
The daemon is configured using the `config.ini` configuration file.
Its implementation can be found under the `rest/src` directory.

n#### Dependencies
The back-end of Pocoweb is written in c++ and depends on the following (ubuntu)
libraries and tools (see also [the dependency listing](misc/docker/dependencies)):
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

#### Compilation
After all dependencies have been installed (see above),
build the back-end using the command `make` (use `-jN` to speed up the
compilation, where `N` stands for the number of parallel build processes).

After the compilation has finished, you should test the back-end.
Execute Pocoweb's unit tests with `make test`.

#### Mysql
Pocoweb uses a mysql database to store its data.
You need to create a database called `pocoweb` with an according user account
and update Pocoweb's configuration file `config.ini`

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

Update and install the unit file `misc/systemd/pocoweb.service`.
You can then manage Pocoweb using `systemd {enable,restart,start,stop} pocoweb`.
It is possible to inspect Pocoweb's log using `journalctl [-f] -u pocoweb`.

### Pocoweb front-end
Pocoweb's front-end is written in PHP using some Javascript functions.
In order to run, an additional web server with support for PHP is needed.
Also the front-end relies on a running Pocoweb back-end process.
The front-end is implemented in the `frontend` directory.

#### Daemon
The front-end needs Pocoweb's daemon to run.
You have to configure the daemon's endpoint in the `frontend/resources/config.php`
file setting the appropriate `$config['backend']['url']` variable.

If the daemon is running on `localhost:8080` for example,
you need to configure redirection URL in your web server and insert
set this URL in the configuration.

#### Installation
You need to install the front-end into your server's web-directory.
Since there are some files generated automatically (including this documentation)
it is not possible to merely copy the `frontend` directory.
If you change any configuration variables in `frontend/resources/config.php`,
make sure to reinstall the front-end.

To install the front-end to `/path/to/web/directory`,
issue the following command: `make install-frontend`:
`make install-frontend PCW_FRONTEND_DIR=/path/to/web/directory`.
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
