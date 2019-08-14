FROM golang:alpine AS build_base
RUN apk add git
WORKDIR /build
COPY go.mod .
COPY go.sum .
RUN go mod download

FROM build_base AS build
COPY . .
RUN CGO_ENABLED=0 go install .

FROM alpine AS pcwauth
COPY --from=build /go/bin/pcwauth /bin/pcwauth
CMD pcwauth \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-pocoweb http://pocoweb \
	-profiler http://pcwprofiler \
	-users http://pcwusers \
    -pkg http://pcwpkg \
	-postcorrection http://pcwpostcorrection \
	-ocr http://pcwocr \
	-listen ':80' \
	-debug
