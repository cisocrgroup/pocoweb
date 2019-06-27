FROM alpine:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue 04 Jun 2019 02:06:35 PM CEST'
#curl-dev
ENV DEPS='boost-dev mariadb-dev leptonica-dev icu-dev mariadb-client bash'
ENV BUILD_DEPS='clang nodejs make build-base cmake php7 graphviz curl'
ENV LANG="C.UTF-8"
ENV LANGUAGE="${LANG}"
ENV LC_ALL="${LANG}"
VOLUME /project-data /www-data /tmp

COPY rest /build/rest
COPY make /build/make
COPY modules /build/modules
COPY frontend /build/frontend
COPY LICENSE Makefile /build/
COPY misc/scripts/md2html.sh /build/misc/scripts/

RUN apk add ${DEPS} ${BUILD_DEPS} \
	&& cd /build \
	&& make CXX=clang++ -j $(nproc) \
	&& make PCW_FRONTEND_DIR=/apps install-frontend \
	&& cp pcwd /apps/pocoweb \
	&& cd / \
	&& rm -rf /build \
    && apk del 	${BUILD_DEPS}

COPY db/tables.sql \
	misc/docker/pocoweb/pocoweb.conf \
	misc/docker/pocoweb/startup.sh \
	/apps/
CMD bash /apps/startup.sh
