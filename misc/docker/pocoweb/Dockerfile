FROM ffink/profiler:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Sa 24. Nov 17:39:12 CET 2018'

VOLUME /project-data /www-data /tmp /language-data
COPY dependencies.txt /dependencies.txt
RUN yum update -y && yum install -y $(cat /dependencies.txt)
# RUN apt-get update && apt-get install -y $(cat /dependencies.txt)

# pocoweb
ENV POCOWEB_CHECKOUT_BRANCH devel
RUN git clone https://git@github.com/cisocrgroup/pocoweb.git \
	--single-branch -b $POCOWEB_CHECKOUT_BRANCH &&\
	cd /pocoweb &&\
	git submodule update --init --recursive &&\
	make V="" CXX=clang++ -j $(nproc) &&\
	make -j $(nproc) V="" PCW_FRONTEND_DIR=/apps install-frontend &&\
	mkdir -p /apps &&\
	cp /pocoweb/pcwd /apps/pocoweb &&\
	cp /pocoweb/db/tables.sql /apps/tables.sql &&\
	cd / &&\
	rm -rf /pocoweb

COPY pocoweb.conf /apps/pocoweb.conf
COPY startup.sh /apps/startup.sh

EXPOSE 8080
CMD sh /apps/startup.sh
