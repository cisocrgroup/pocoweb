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
Pocoweb is a postcorrection tool for the web.
It is based on [PoCoTo](https://github.com/cisocrgroup/PoCoTo).

## Dependencies
 * g++-5
 * libbost-dev
 * make
 * libssl-dev
 * libmariadbclient (mariadb is not needed)

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
