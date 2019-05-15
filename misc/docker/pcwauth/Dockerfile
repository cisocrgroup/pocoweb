FROM golang:alpine
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Mi 15. Mai 22:52:45 CEST 2019'

ENV PCWAUTH_GIT=github.com/finkf/pcwauth
ENV GO111MODULE=on
RUN apk add git &&\
	go get -u ${PCWAUTH_GIT} &&\
	apk del git

CMD sleep 15 && pcwauth -db "${MYSQL_USER}:${MYSQL_PASSWORD}@(${PCW_DB_HOST})/${MYSQL_DATABASE}" \
	-pocoweb http://pocoweb:8080 \
	-profiler http://pcwprofiler \
	-listen ':80' \
	-root-name ${PCW_ROOT_NAME} \
	-root-password ${PCW_ROOT_PASSWORD} \
	-root-email ${PCW_ROOT_EMAIL} \
	-root-institute ${PCW_ROOT_INSTITUTE} \
	-debug
