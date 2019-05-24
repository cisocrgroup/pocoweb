FROM ffink/profiler:internal-backend
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Fri 24 May 2019 03:36:48 PM CEST'
ENV GITURL='https://github.com/cisocrgroup/pocoweb.git'
ENV BRANCH='flodev'

VOLUME /project-data /www-data /tmp
COPY dependencies.txt /dependencies.txt
RUN yum update -y && yum install -y $(cat /dependencies.txt)
# RUN apt-get update && apt-get install -y $(cat /dependencies.txt)

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

EXPOSE 8080
CMD sh /apps/startup.sh
