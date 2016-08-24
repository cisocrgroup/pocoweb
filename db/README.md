* CREATE USER pocoweb IDENTIFIED BY 'pocoweb-password';
* UPDATE mysql.user SET password=PASSWORD('new-password') WHERE user='pocoweb';
* DROP USER pocoweb;
* GRANT ALL ON pocoweb.* to user pocoweb;

