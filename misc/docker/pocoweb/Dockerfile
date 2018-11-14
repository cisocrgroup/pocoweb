FROM profiler:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>

VOLUME /project-data /www-data /tmp /language-data
COPY dependencies /dependencies
RUN yum update -y && yum install -y $(cat /dependencies)

# pocoweb
ENV POCOWEB_CHECKOUT_BRANCH devel
RUN git clone https://git@github.com/cisocrgroup/pocoweb.git \
	--single-branch -b $POCOWEB_CHECKOUT_BRANCH &&\
	cd /pocoweb &&\
	git submodule update --init --recursive &&\
	make V="" CXX=clang++ -j $(nproc) &&\
	make -j $(nproc) V="" PCW_FRONTEND_DIR=/www-data install-frontend &&\
	mkdir -p /apps &&\
	cp /pocoweb/pcwd /apps/pocoweb &&\
	cp /pocoweb/db/tables.sql /apps/tables.sql &&\
	cd / &&\
	rm -rf /pocoweb

COPY pocoweb.conf /apps/pocoweb.conf
COPY startup.sh /startup.sh

EXPOSE 8080
CMD sleep 5 && /startup.sh
