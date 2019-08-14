FROM alpine:latest AS deps
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue 04 Jun 2019 02:06:35 PM CEST'
ENV DEPS='boost-dev mariadb-dev leptonica-dev icu-dev mariadb-client bash zip'
ENV BUILD_DEPS='clang nodejs make build-base cmake php7 graphviz curl'
ENV LANG="C.UTF-8"
ENV LANGUAGE="${LANG}"
ENV LC_ALL="${LANG}"

RUN apk add ${DEPS} ${BUILD_DEPS}

FROM deps AS frontend
COPY LICENSE Makefile /build/
COPY make /build/make
COPY misc/scripts/md2html.sh /build/misc/scripts/
COPY frontend /build/frontend
RUN cd /build \
	&& make PCW_FRONTEND_DIR=/apps install-frontend

FROM frontend AS libs
COPY modules /build/modules
RUN cd /build \
	&& make CXX=clang++ -j $(nproc) lib/libpugixml.a lib/libsqlpp-mysql.a

FROM libs
VOLUME /project-data /www-data /tmp
COPY rest /build/rest
RUN cd /build \
	&& make CXX=clang++ -j $(nproc) \
	&& cp pcwd /apps/pocoweb \
	&& cd / \
	&& rm -rf /build \
	&& apk del ${BUILD_DEPS}

COPY db/tables.sql \
	misc/docker/pocoweb/pocoweb.conf \
	misc/docker/pocoweb/startup.sh \
	/apps/
CMD bash /apps/startup.sh
