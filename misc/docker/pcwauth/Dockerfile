FROM golang:1.11-alpine
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue Jan  8 19:57:50 CET 2019'

ENV PCWAUTH_GIT=github.com/finkf/pcwauth
RUN apk add git &&\
	go get ${PCWAUTH_GIT} &&\
	go install ${PCWAUTH_GIT} &&\
	rm -rf src &&\
	apk del git
CMD sleep 15 && pcwauth -db "${MYSQL_USER}:${MYSQL_PASSWORD}@(${PCW_DB_HOST})/${MYSQL_DATABASE}" \
	-pocoweb ${PCW_POCOWEB_HOST} -listen ${PCW_PCWAUTH_HOST} \
	-root-name ${PCW_ROOT_NAME} -root-password ${PCW_ROOT_PASSWORD} \
    -root-email ${PCW_ROOT_EMAIL} -root-institute ${PCW_ROOT_INSTITUTE} \
	-debug
