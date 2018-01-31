FROM nginx:alpine
MAINTAINER Florian Fink <fink@cis.lmu.de>

COPY html /usr/share/nginx/
COPY nginx.conf /etc/nginx/nginx.conf
COPY fastcgi-php.conf /etc/nginx/snippets/fastcgi-php.conf

EXPOSE 80
CMD ["nginx-debug", "-g", "daemon off;"]
