FROM alpine:latest AS deps
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue 04 Jun 2019 02:06:35 PM CEST'
ENV DEPS='boost-dev mariadb-dev leptonica-dev icu-dev mariadb-client bash zip'
ENV BUILD_DEPS='clang nodejs make build-base cmake php7 graphviz curl'
ENV LANG="C.UTF-8"
ENV LANGUAGE="${LANG}"
ENV LC_ALL="${LANG}"

RUN apk add ${DEPS} ${BUILD_DEPS}

FROM deps AS libs
COPY modules /build/modules
COPY LICENSE Makefile /build/
COPY make /build/make
RUN cd /build \
	&& make CXX=clang++ -j $(nproc) lib/libpugixml.a lib/libsqlpp-mysql.a \
	&& make modules-clean

FROM libs as backend
VOLUME /project-data /www-data /tmp
COPY rest /build/rest
RUN cd /build \
	&& make CXX=clang++ -j $(nproc) \
	&& mkdir /apps/ \
	&& cp pcwd /apps/pocoweb \
	&& cd / \
	&& rm -rf /build

FROM backend AS frontend
COPY frontend /build/frontend
RUN cd /build \
	&& make -C frontend install \
	&& cd / \
	&& rm -rf /build \
	&& apk del ${BUILD_DEPS}

FROM frontend as RUN
#	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
COPY db/tables.sql misc/scripts/startup.sh /apps/
CMD bash /apps/startup.sh \
	-m db \
	-p $MYSQL_PASSWORD \
	-n $MYSQL_DATABASE \
	-u $MYSQL_USER \
	-b /project-data \
	-l "0.0.0.0:80"
