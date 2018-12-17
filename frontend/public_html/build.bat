rem ========================
rem build script for windows
rem ========================

@echo off

cd .\assets\js
node r.js -o build.js

cd ..\..\

rem scp -r . tobiasenglmeier@tobiass-macbook-pro-2.fritz.box:/Users/tobiasenglmeier/pocoweb_data/www-data/public_html

xcopy /s /Y C:\pocoweb\frontend\public_html C:\pocoweb_data\www-data\public_html

@echo on

