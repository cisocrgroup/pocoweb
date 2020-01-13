# FROM alpine:3.10
# MAINTAINER Florian Fink <finkf@cis.lmu.de>
# ENV VERSION='Mi 19. Jun 20:02:39 CEST 2019'
# ENV DEPS='python3 py3-numpy-f2py py-numpy-dev py3-scipy py3-pillow py3-prettytable py3-setuptools libxml2-dev libxslt-dev freetype-dev hdf5-dev@testing python3-dev py3-matplotlib@testing py3-kiwisolver@testing py3-cycler@testing py3-pip'
# ENV BUILD_DEPS='gcc g++ musl-dev'
# ENV PATH="${PATH}:/go/bin"
# ENV TESTREPO='http://nl.alpinelinux.org/alpine/edge/testing'

# RUN echo "@testing ${TESTREPO}" >> /etc/apk/repositories \
# 	&& apk add ${DEPS} ${BUILD_DEPS} \
# 	&& pip3 install --upgrade pip \
# 	&& pip install scikit-image \
# 	&& pip3 install	calamari_ocr \
# 	&& apk del ${BUILD_DEPS}

# COPY pcwocr /go/bin/
# COPY testdata/config.toml /etc/pcwocr.toml
# CMD pcwocr \
#     -listen :80 \
# 	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
# 	-models /data/models \
# 	-base / \
# 	-debug

FROM golang:latest
MAINTAINER Florian Fink <finkf@cis.lmu.de>
ENV PATH="${PATH}:/usr/local/bin"
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update \
	&& apt-get -y install python3-pip python3-dev build-essential locales \
	&& sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen \
    && dpkg-reconfigure --frontend=noninteractive locales \
    && update-locale LANG=en_US.UTF-8 \
	&& pip3 install tensorflow \
	&& pip3 install calamari_ocr

ENV LANG="en_US.UTF-8"
ENV LANGUAGE="${LANG}"
ENV LC_ALL="${LANG}"
COPY testdata/calamari.patch /patch/
RUN patch /usr/local/lib/python3.5/dist-packages/calamari_ocr/scripts/resume_training.py /patch/calamari.patch \
	&& rm -rf /patch

COPY pcwocr /go/bin/pcwocr
COPY testdata/config.toml /etc/pcwocr.toml
VOLUME ["/project-data", "/data/models"]
CMD pcwocr \
    -listen :80 \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-models /data/models \
	-base / \
	-debug
