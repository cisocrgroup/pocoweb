FROM ubuntu:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>

## dependencies
COPY dependencies /dependencies
RUN apt-get update && apt-get install -y $(cat /dependencies)

## pocoweb
ENV POCOWEB_BRANCH "doc"
RUN git clone https://git@github.com/cisocrgroup/pocoweb.git /pocoweb
WORKDIR /pocoweb
RUN git checkout $POCOWEB_BRANCH && git submodule update --init --recursive
RUN make -j $(nproc)
# RUN make -j test

## profiler
RUN git clone https://git@github.com/cisocrgroup/Profiler.git /profiler
WORKDIR /profiler
RUN mkdir build
WORKDIR /profiler/build
RUN cmake ..
RUN make -j $(nproc)

## clean up
WORKDIR /
COPY config.ini /apps/pocoweb.conf
RUN cp /profiler/build/bin/profiler /apps/profiler
RUN cp /pocoweb/pcwd /apps/pocoweb
RUN cp /pocoweb/lib/simple-login.so /apps/
RUN rm -rf /profiler /pocoweb

## run pocoweb
EXPOSE 80
CMD /apps/pocoweb /apps/pocoweb.conf
