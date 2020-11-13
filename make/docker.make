PCW_BASE_DIR ?= /srv/pocoweb
TAG ?= flobar/pocoweb
ifeq (, ${shell which git})
TAGS := latest
else
TAGS := ${addprefix ${TAG}:,${shell git describe --tags HEAD} latest}
endif

.PHONY: docker-build
docker-build: services-build
	docker build ${addprefix -t,${TAGS}} .

.PHONY: docker-push
docker-push: services-push
	for t in ${TAGS}; do docker push $$t; done

.PHONY: services-push
services-push:
	${MAKE} -C services docker-push

.PHONY: services-build
services-build:
	${MAKE} -C services docker-build

.PHONY: docker-compose-build
docker-compose-build: env.sh services/nginx/cert.pem
	PCW_BASE_DIR=${PCW_BASE_DIR} docker-compose build

.PHONY: docker-start
docker-start: install-frontend docker-compose-build
	PCW_BASE_DIR=${PCW_BASE_DIR} docker-compose up -d

.PHONY: install-frontend
install-frontend:
	$(MAKE) -C frontend INSTALL_DIR=${PCW_BASE_DIR}/www-data install

.PHONY: docker-stop
docker-stop: env.sh
	PCW_BASE_DIR=${PCW_BASE_DIR} docker-compose stop

env.sh: misc/config/env.sh
	cp $< $@
services/nginx/cert.pem:
	sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout services/nginx/key.pem -out $@
