FROM ffink/profiler:internal-backend
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Tue 04 Jun 2019 02:06:33 PM CEST'
ENV GITURL='https://github.com/cisocrgroup/pocoweb.git'
ENV BRANCH='devel'

VOLUME /project-data /www-data /tmp
COPY deps-fedora.txt /deps.txt
RUN yum update -y && yum install -y $(cat /deps.txt | grep -v '^#')

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
