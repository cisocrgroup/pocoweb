FROM golang:alpine AS build_base
RUN apk add git
WORKDIR /build
COPY go.mod .
COPY go.sum .
RUN go mod download

FROM build_base AS build
COPY . .
RUN CGO_ENABLED=0 go install .

FROM alpine AS pcwusers
COPY --from=build /go/bin/pcwusers /bin/pcwusers
CMD pcwusers \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-listen ':80' \
	-root-name ${PCW_ROOT_NAME} \
	-root-password ${PCW_ROOT_PASSWORD} \
	-root-email ${PCW_ROOT_EMAIL} \
	-root-institute ${PCW_ROOT_INSTITUTE} \
	-debug
