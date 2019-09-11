FROM alpine:latest

RUN apk add libressl certbot \
	&& mkdir -p /etc/letsencrypt/live/pocoweb.cis.lmu.de \
	&& mkdir -p /var/www/letsencrypt/live/pocoweb.cis.lmu.de \
	&& openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
	-keyout /etc/letsencrypt/live/pocoweb.cis.lmu.de/privkey.pem \
	-out /etc/letsencrypt/live/pocoweb.cis.lmu.de/fullchain.pem \
	-subj /CN=pocoweb.cis.lmu.de \
	&& apk del libressl
