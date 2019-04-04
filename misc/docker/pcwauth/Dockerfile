FROM golang:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Do 4. Apr 10:45:21 CEST 2019'

ENV PCWAUTH_GIT=github.com/finkf/pcwauth
ENV GO111MODULE=on
RUN git clone https://${PCWAUTH_GIT} &&\
    cd pcwauth &&\
	go install . &&\
	cd / &&\
    rm -rf pcwauth

CMD sleep 15 && pcwauth -db "${MYSQL_USER}:${MYSQL_PASSWORD}@(${PCW_DB_HOST})/${MYSQL_DATABASE}" \
	-pocoweb ${PCW_POCOWEB_HOST} -listen ${PCW_PCWAUTH_HOST} \
	-root-name ${PCW_ROOT_NAME} -root-password ${PCW_ROOT_PASSWORD} \
    -root-email ${PCW_ROOT_EMAIL} -root-institute ${PCW_ROOT_INSTITUTE} \
	-debug
