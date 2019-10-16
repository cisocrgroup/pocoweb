FROM flobar/ocrd_cis:latest
MAINTAINER Florian Fink
ENV VERSION 'Fri 14 Jun 2019 04:57:21 PM CEST'
VOLUME "/project-data"

COPY testdata/run.bash /apps/
COPY testdata/pcwpostcorrection.json /etc/skel/
COPY pcwpostcorrection /usr/bin/
WORKDIR /
ENTRYPOINT /usr/bin/pcwpostcorrection \
	-debug \
	-dsn "${MYSQL_USER}:${MYSQL_PASSWORD}@(db)/${MYSQL_DATABASE}" \
	-config "${DATA}/config/ocrd-cis-post-correction.json" \
    -pocoweb "http://pocoweb" \
	-base /
