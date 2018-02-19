FROM nginx:alpine
MAINTAINER Florian Fink <fink@cis.lmu.de>

VOLUME /www-data /project-data
COPY nginx.conf /etc/nginx/nginx.conf
COPY fastcgi-php.conf /etc/nginx/snippets/fastcgi-php.conf

EXPOSE 80
CMD ["nginx-debug", "-g", "daemon off;"]
