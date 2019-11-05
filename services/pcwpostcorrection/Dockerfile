FROM golang:alpine AS build_base
RUN apk add git
WORKDIR /build
COPY go.mod .
COPY go.sum .
RUN go mod download

FROM build_base AS build
COPY testdata/run.bash /apps/
COPY testdata/pcwpostcorrection.json /etc/skel/
WORKDIR /build
COPY . .
RUN CGO_ENABLED=0 go install .

FROM flobar/ocrd_cis:latest
MAINTAINER Florian Fink
ENV VERSION 'Fri 14 Jun 2019 04:57:21 PM CEST'
VOLUME "/project-data"
COPY --from=build /go/bin/pcwpostcorrection /usr/bin/
COPY --from=build /apps/run.bash /apps/
COPY --from=build /etc/skel/pcwpostcorrection.json /etc/skel/

WORKDIR /
ENTRYPOINT /usr/bin/pcwpostcorrection \
	-debug \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
    -pocoweb "http://pocoweb" \
	-base /
