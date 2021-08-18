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

FROM flobar/profiler:debian
LABEL MAINTAINER Florian Fink
ENV VERSION='Wed 18 Aug 2021 12:01:27 PM CEST'
ENV GITURL="https://github.com/cisocrgroup"
ENV PATH="${PATH}:/go/bin"

COPY --from=build /go/bin/pcwprofiler /bin/pcwprofiler
CMD pcwprofiler \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-profiler /apps/profiler \
	-language-dir /language-data \
	-base / \
    	-debug
