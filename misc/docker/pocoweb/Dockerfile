FROM profiler:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>

VOLUME /project-data /www-data /tmp /language-data
## dependencies
COPY dependencies /dependencies
RUN apt-get update && apt-get install -y $(cat /dependencies)

# pocoweb
ENV POCOWEB_BRANCH "doc"
RUN git clone https://git@github.com/cisocrgroup/pocoweb.git /pocoweb &&\
		cd /pocoweb &&\
		git checkout $POCOWEB_BRANCH &&\
		git submodule update --init --recursive &&\
		make -j $(nproc) &&\
		make -j $(nproc) V="" PCW_FRONTEND_DIR=/www-data install-frontend &&\
		mkdir -p /apps &&\
		cp /pocoweb/pcwd /apps/pocoweb &&\
		cp /pocoweb/lib/simple-login.so /apps/simple-login.so &&\
		cp /pocoweb/db/tables.sql /apps/tables.sql &&\
		cd / &&\
		rm -rf /pocoweb

COPY pocoweb.conf /apps/pocoweb.conf
COPY startup.sh /startup.sh

EXPOSE 8080
CMD /startup.sh
