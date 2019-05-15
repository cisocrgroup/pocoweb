FROM golang:alpine
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Wed 15 May 2019 05:13:08 PM CEST'

ENV PCWAUTH_GIT=github.com/finkf/pcwauth
ENV GO111MODULE=on
RUN apk add git &&\
	go get -u ${PCWAUTH_GIT} &&\
	apk del git

CMD sleep 15 && pcwauth -db "${MYSQL_USER}:${MYSQL_PASSWORD}@(${PCW_DB_HOST})/${MYSQL_DATABASE}" \
	-pocoweb ${PCW_POCOWEB_HOST} -listen ${PCW_PCWAUTH_HOST} \
	-root-name ${PCW_ROOT_NAME} -root-password ${PCW_ROOT_PASSWORD} \
    -root-email ${PCW_ROOT_EMAIL} -root-institute ${PCW_ROOT_INSTITUTE} \
	-debug
