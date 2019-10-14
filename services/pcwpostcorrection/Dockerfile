FROM flobar/ocrd-cis:latest
MAINTAINER Florian Fink
ENV VERSION 'Fri 14 Jun 2019 04:57:21 PM CEST'
VOLUME "/project-data"

COPY testdata/run_rrdm.bash testdata/run_el.bash /apps/
COPY pcwpostcorrection /go/bin/
WORKDIR /
ENTRYPOINT /go/bin/pcwpostcorrection \
	-debug \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-config "${DATA}/config/ocrd-cis-post-correction.json" \
    -pocoweb "http://pocoweb" \
	-base /
