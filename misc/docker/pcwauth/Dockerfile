FROM golang:1.11-alpine
MAINTAINER Florian Fink <finkf@cis.lmu.de>

ENV xxx=xxxyyy
ENV PCWAUTH_GIT=github.com/finkf/pcwauth
RUN apk add git &&\
	go get ${PCWAUTH_GIT} &&\
	go install ${PCWAUTH_GIT} &&\
	rm -rf src &&\
	apk del git
CMD sleep 5 && pcwauth -db "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-pocoweb ${PCW_POCOWEB_HOST} -listen ${PCW_PCWAUTH_HOST} -debug
