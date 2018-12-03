FROM golang:1.11-alpine
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Sa 24. Nov 17:34:01 CET 2018'

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
