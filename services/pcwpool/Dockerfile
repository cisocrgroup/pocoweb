FROM golang:alpine AS build_base
RUN apk add git
WORKDIR /build
COPY go.mod .
COPY go.sum .
RUN go mod download

FROM build_base AS build
COPY . .
RUN CGO_ENABLED=0 go install .

FROM alpine AS pcwpool
COPY --from=build /go/bin/pcwpool /bin/pcwpool
CMD pcwpool \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-listen ':80' \
    -base / \
	-debug
