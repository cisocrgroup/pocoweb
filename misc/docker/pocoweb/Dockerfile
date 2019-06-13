FROM ubuntu:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue 04 Jun 2019 02:06:35 PM CEST'
ENV GITURL='https://github.com/cisocrgroup/pocoweb.git'
ENV BRANCH='flodev'
ENV DEBIAN_FRONTEND noninteractive

COPY deps-ubuntu.txt /deps.txt
RUN apt-get update \
	&& apt-get -y install $(cat /deps.txt | grep -v '^#')
# locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    dpkg-reconfigure --frontend=noninteractive locales && \
    update-locale LANG=en_US.UTF-8

VOLUME /project-data /www-data /tmp

# pocoweb
RUN git clone ${GITURL} --single-branch -b ${BRANCH} \
	&& cd /pocoweb \
	&& git submodule update --init --recursive \
	&& make CXX=clang++ -j $(nproc) \
	&& make PCW_FRONTEND_DIR=/apps install-frontend \
	&& mkdir -p /apps \
	&& cp /pocoweb/pcwd /apps/pocoweb \
	&& cp /pocoweb/db/tables.sql /apps/tables.sql \
	&& cd / \
	&& rm -rf /pocoweb

COPY pocoweb.conf /apps/pocoweb.conf
COPY startup.sh /apps/startup.sh

CMD sh /apps/startup.sh
