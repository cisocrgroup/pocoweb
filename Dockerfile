FROM ubuntu:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue 04 Jun 2019 02:06:35 PM CEST'
ENV GITURL='https://github.com/cisocrgroup/pocoweb.git'
ENV BRANCH='flodev'
ENV DEBIAN_FRONTEND noninteractive
VOLUME /project-data /www-data /tmp

COPY misc/docker/pocoweb/deps-ubuntu.txt /build/deps.txt
RUN apt-get update \
	&& apt-get -y install $(cat /build/deps.txt | grep -v '^#')
# locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    dpkg-reconfigure --frontend=noninteractive locales && \
    update-locale LANG=en_US.UTF-8

COPY rest /build/rest
COPY make /build/make
COPY modules /build/modules
COPY frontend /build/frontend
COPY LICENSE Makefile /build/
COPY misc/scripts/md2html.sh /build/misc/scripts/

RUN cd /build \
	&& make CXX=clang++ -j $(nproc) \
	&& make PCW_FRONTEND_DIR=/apps install-frontend \
	&& cp pcwd /apps/pocoweb \
	&& cd / \
	&& rm -rf /build

COPY db/tables.sql \
	misc/docker/pocoweb/pocoweb.conf \
	misc/docker/pocoweb/startup.sh \
	/apps/
CMD bash /apps/startup.sh
