FROM golang:1.11-alpine
MAINTAINER Florian Fink <finkf@cis.lmu.de>

ENV PCWAUTH_GIT=github.com/finkf/pcwauth
RUN apk add git &&\
	go get $PCWAUTH_GIT &&\
	go install $PCWAUTH_GIT &&\
	rm -rf src &&\
    apk del git
CMD sleep 5 && pcwauth -debug -db $PCWAUTH_DB -pocoweb $PCWAUTH_POCOWEB -listen $PCWAUTH_HOST
