FROM alpine:3.13 AS deps
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue 04 Jun 2019 02:06:35 PM CEST'
ENV DEPS='boost-dev mariadb-dev leptonica-dev icu-dev mariadb-client bash zip'
ENV BUILD_DEPS='clang make build-base cmake'
ENV LANG="C.UTF-8"
ENV LANGUAGE="${LANG}"
ENV LC_ALL="${LANG}"

RUN apk --update add ${DEPS} ${BUILD_DEPS}

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
COPY db /build/db
RUN cd /build \
	&& make CXX=clang++ -j $(nproc) install \
	&& cd / \
	&& rm -rf /build \
 	&& apk del ${BUILD_DEPS}

FROM backend as RUN
COPY misc/scripts/startup.sh /apps/
CMD bash /apps/startup.sh \
	-m db \
	-p $MYSQL_PASSWORD \
	-n $MYSQL_DATABASE \
	-u $MYSQL_USER \
	-b /project-data \
	-l "0.0.0.0:80"
