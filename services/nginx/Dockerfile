FROM nginx:alpine
MAINTAINER Florian Fink <fink@cis.lmu.de>

VOLUME /www-data /project-data
COPY cert.pem /etc/ssl/pocoweb.cis.lmu.de.pem
COPY key.pem /etc/ssl/pocoweb.cis.lmu.de.key
COPY nginx.conf /etc/nginx/nginx.conf

EXPOSE 80
EXPOSE 443
CMD ["nginx-debug", "-g", "daemon off;"]
