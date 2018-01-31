FROM nginx:alpine
MAINTAINER Florian Fink <fink@cis.lmu.de>
ENV VERSION 0.6

COPY nginx.conf /etc/nginx/nginx.conf
COPY default /etc/nginx/sites-enabled/default
COPY fastcgi-php.conf /etc/nginx/snippets/fastcgi-php.conf
COPY html /usr/share/nginx/

EXPOSE 80
#CMD ["nginx", "-g", "daemon off;"]
CMD /bin/sh
