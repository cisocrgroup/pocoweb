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
		make -j $(nproc) profiler &&\
		mkdir -p /apps &&\
		cp bin/profiler /apps/profiler &&\
		cd / &&\
		rm -rf /profiler

# locale for profiler
ENV LANG C.UTF-8
ENV LANGUAGE C.UTF-8
ENV LC_ALL C.UTF-8
