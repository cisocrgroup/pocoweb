FROM golang:alpine AS build_base
RUN apk add git
WORKDIR /build
COPY go.mod .
COPY go.sum .
RUN go mod download

FROM build_base AS build
WORKDIR /build
COPY . .
RUN CGO_ENABLED=0 go install .

FROM flobar/profiler:latest
MAINTAINER Florian Fink
ENV VERSION='Fri 07 Jun 2019 01:56:35 PM CEST'
ENV GITURL="https://github.com/cisocrgroup"
ENV PATH="${PATH}:/go/bin"
ENV LC_ALL=C

COPY --from=build /go/bin/pcwprofiler /bin/pcwprofiler
CMD pcwprofiler \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-profiler /apps/profiler \
	-language-dir /language-data \
	-base / \
    -debug
