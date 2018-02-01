FROM ubuntu:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>

## dependencies
COPY dependencies /dependencies
RUN apt-get update && apt-get install -y $(cat /dependencies)

## pocoweb
ENV POCOWEB_BRANCH "doc"
RUN git clone https://git@github.com/cisocrgroup/pocoweb.git /pocoweb &&\
		cd /pocoweb &&\
		git checkout $POCOWEB_BRANCH &&\
		git submodule update --init --recursive &&\
		make -j $(nproc)

## profiler
RUN git clone https://git@github.com/cisocrgroup/Profiler.git /profiler &&\
		cd /profiler &&\
		mkdir build &&\
		cd build &&\
		cmake .. &&\
		make -j $(nproc)

## clean up
WORKDIR /
COPY pocoweb.conf /apps
RUN cp /profiler/build/bin/profiler /apps/profiler &&\
		cp /pocoweb/pcwd /apps/pocoweb &&\
		cp /pocoweb/lib/simple-login.so apps &&\
		rm -rf /profiler &&\
		rm -rf /pocoweb

## run pocoweb
EXPOSE 8080
CMD /apps/pocoweb /apps/pocoweb.conf
