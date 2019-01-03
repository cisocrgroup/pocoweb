FROM fedora:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV DATE='Fri Nov 23 15:48:47 CET 2018'

## dependencies
COPY dependencies.txt /dependencies.txt
#RUN apt-get update && apt-get install -y $(cat /dependencies)
RUN yum update -y && yum install -y $(cat /dependencies.txt)

## profiler
RUN git clone https://git@github.com/cisocrgroup/Profiler.git /profiler &&\
		mkdir /profiler/build &&\
		cd /profiler/build &&\
		cmake .. &&\
	    make -j $(nproc) profiler &&\
	    make -j $(nproc) trainFrequencyList &&\
	    make -j $(nproc) compileFBDic &&\
		mkdir -p /apps &&\
		cp bin/profiler /apps/profiler &&\
     	cp bin/trainFrequencyList /apps/trainFrequencyList &&\
        cp bin/compileFBDic /apps/compileFBDic &&\
	    cd / &&\
		rm -rf /profiler

# locale for profiler
ENV LANG C.UTF-8
ENV LANGUAGE C.UTF-8
ENV LC_ALL C.UTF-8

## language backend (temp)
RUN git clone https://git@github.com/cisocrgroup/Resources.git /resources &&\
	cd /resources/lexica &&\
	make TRAIN=/apps/trainFrequencyList FBDIC=/apps/compileFBDic &&\
    mkdir /language-data &&\
	cp -r greek /language-data/ &&\
	cp -r german /language-data/ &&\
	cp -r latin /language-data/ &&\
	cd / &&\
	rm -rf /resources
