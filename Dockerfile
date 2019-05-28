FROM ubuntu:18.04

# FROM ffink/profiler:internal-backend
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Fri 24 May 2019 03:36:48 PM CEST'
ENV TZ 'Europe/Berlin'
ENV DEBIAN_FRONTEND noninteractive

VOLUME /project-data /www-data /tmp
COPY misc/docker/pocoweb/deps-ubuntu.txt /deps.txt

RUN apt-get update \
	&& apt-get install -y tzdata \
	&& ln -f /usr/share/zoneinfo/${TZ} /etc/localtime \
	&& dpkg-reconfigure --frontend noninteractive tzdata \
	&& apt-get install -y $(cat /deps.txt | grep -v '^#')
COPY . /pocoweb
RUN ls -Rl /pocoweb
RUN cd /pocoweb \
	&& make CXX=g++ -j $(nproc) \
	&& make PCW_FRONTEND_DIR=/apps install-frontend \
	&& cp /pocoweb/pcwd /apps/pocoweb \
	&& cp /pocoweb/db/tables.sql /apps/tables.sql \
	&& cd /

COPY misc/docker/pocoweb/pocoweb.conf /apps/pocoweb.conf
COPY misc/docker/pocoweb/startup.sh /apps/startup.sh

EXPOSE 8080
CMD sh /apps/startup.sh
