FROM golang:1.11-alpine
MAINTAINER Florian Fink <finkf@cis.lmu.de>

ENV PCWAUTH_GIT=github.com/finkf/pcwauth
ENV DSN=$MYSQL_USER:$MYSQL_PASSWORD@($PCW_DB_HOST)/$MYSQL_DATABASE
RUN apk add git &&\
	go get $PCWAUTH_GIT &&\
	go install $PCWAUTH_GIT &&\
	rm -rf src &&\
    apk del git
CMD sleep 5 && pcwauth -debug -db $DSN -pocoweb $PCW_POCOWEB_HOST -listen $PCW_PCWAUTH_HOST
