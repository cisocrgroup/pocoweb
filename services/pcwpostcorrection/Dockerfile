FROM golang:1.13-alpine
ENV BUILD_DEPS 'git gcc tesseract-ocr-dev musl-dev'
ENV DEPS 'tesseract-ocr-data-deu'
RUN apk update && apk add ${BUILD_DEPS} ${DEPS}
RUN mkdir -p /models \
    && wget -O /models/pre19th.bin http://cis.lmu.de/~finkf/apoco/models/pre19th-general-courageous.bin \
    && wget -O /models/19th.bin http://cis.lmu.de/~finkf/apoco/models/19th-general-courageous.bin
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
    -model /models \
    -tess /usr/share/tessdata
