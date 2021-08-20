FROM golang:1.13-alpine AS dependencies
RUN apk add git
WORKDIR /build
COPY go.mod .
COPY go.sum .
RUN go mod download

FROM dependencies AS build
COPY . .
RUN CGO_ENABLED=0 go install .

FROM alpine AS pcwauth
COPY --from=build /go/bin/pcwauth /go/bin/pcwauth
COPY --from=build /etc/ssl/certs/ca-certificates.crt /etc/ssl/certs/ca-certificates.crt
CMD /go/bin/pcwauth \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-pocoweb http://pocoweb \
	-profiler http://pcwprofiler \
	-users http://pcwusers \
	-pkg http://pcwpkg \
    -pool http://pcwpool \
	-postcorrection http://pcwpostcorrection \
	-listen ':80' \
	-debug
