FROM golang:alpine
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='So 7. Apr 03:00:07 CEST 2019'

ENV PCWAUTH_GIT=github.com/finkf/pcwauth
RUN apk add git &&\
	go get -u ${PCWAUTH_GIT} &&\
	apk del git
CMD sleep 15 && pcwauth -db "${MYSQL_USER}:${MYSQL_PASSWORD}@(${PCW_DB_HOST})/${MYSQL_DATABASE}" \
	-pocoweb ${PCW_POCOWEB_HOST} -listen ${PCW_PCWAUTH_HOST} \
	-root-name ${PCW_ROOT_NAME} -root-password ${PCW_ROOT_PASSWORD} \
    -root-email ${PCW_ROOT_EMAIL} -root-institute ${PCW_ROOT_INSTITUTE} \
	-debug
