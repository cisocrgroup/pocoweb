FROM golang:1.13-alpine
ENV BUILD_DEPS 'git gcc tesseract-ocr-dev musl-dev'
ENV DEPS 'tesseract-ocr-data-deu'
RUN apk update && apk add ${BUILD_DEPS} ${DEPS}
RUN mkdir -p /model \
	&& wget -O /model/pre19th.bin http://cis.lmu.de/~finkf/pre19th \
	&& wget -O /model/19th.bin http://cis.lmu.de/~finkf/19th.bin
WORKDIR /build
COPY go.mod go.sum ./
RUN go mod download
COPY . .
RUN go install .
RUN apk del ${BUILD_DEPS}

VOLUME "/project-data"
WORKDIR /
ENTRYPOINT /go/bin/pcwpostcorrection \
	-debug \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
    -pocoweb "http://pocoweb" \
	-base / \
	-model /model \
	-tess /usr/share/tessdata 
