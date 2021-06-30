rem ========================
rem build script for windows
rem ========================
@echo off

cd .\assets\js
node r.js -o build.js

cd ..\..\
rem copy ..\resources\config.php .\
rem copy ..\resources\library\backend.php .\
rem copy ..\resources\library\api.php .\

rem scp -r . tobiasenglmeier@tobiass-macbook-pro-2.fritz.box:/Users/tobiasenglmeier/pocoweb_data/www-data/public_html

rem pscp -r -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/ tobias@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/

pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/assets/js/require_main.built.js englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/assets/js/
pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/assets/css/custom.css englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/assets/css/
pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/index.html englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/
pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/assets/images/pocoweb_logo.png englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/assets/images/
pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/assets/images/bg_page.jpg englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/assets/images/
pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/assets/images/bg_repeat.png englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/assets/images/

rem pscp -r -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/assets/images/doc/ englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/assets/images/doc/

pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/api_doc.html englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/
pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/manual.html englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/
pscp -pw %pocoweb_pw% C:/pocoweb/frontend/public_html/legal_notice.html englmeier@pocoweb.cis.lmu.de:/srv/pocoweb/www-data/public_html/


rem scp -r . tobiasenglmeier@t-macbook-pro.fritz.box:/Users/tobiasenglmeier/pocoweb_data/www-data/public_html

rem xcopy /s /Y C:\pocoweb\frontend\public_html C:\pocoweb_data\www-data\public_html > nul

@echo on
