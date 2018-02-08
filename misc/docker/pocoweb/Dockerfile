FROM ubuntu:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>

## dependencies
COPY dependencies /dependencies
RUN apt-get update && apt-get install -y $(cat /dependencies)

## profiler
RUN git clone https://git@github.com/cisocrgroup/Profiler.git /profiler &&\
		mkdir /profiler/build &&\
		cd /profiler/build &&\
		cmake .. &&\
		make -j $(nproc) &&\
		mkdir -p /apps &&\
		cp bin/profiler /apps/profiler &&\
		cd / &&\
		rm -rf /profiler

# locale for profiler
ENV LANG C.UTF-8
ENV LANGUAGE C.UTF-8
ENV LC_ALL C.UTF-8

## pocoweb
ENV POCOWEB_BRANCH "doc"
RUN git clone https://git@github.com/cisocrgroup/pocoweb.git /pocoweb &&\
		cd /pocoweb &&\
		git checkout $POCOWEB_BRANCH &&\
		git submodule update --init --recursive &&\
		make -j $(nproc) &&\
		mkdir -p /apps &&\
		cp /pocoweb/pcwd /apps/pocoweb &&\
		cp /pocoweb/lib/simple-login.so /apps/simple-login.so &&\
		cd / &&\
		rm -rf /pocoweb

## pocoweb config file
COPY pocoweb.conf /apps/pocoweb.conf

## run pocoweb
EXPOSE 8080
CMD /apps/pocoweb /apps/pocoweb.conf
