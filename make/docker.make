PCW_SRV_DIR ?= /srv/pocoweb
TAG ?= flobar/pocoweb
ifeq (, ${shell which git})
TAGS := latest
else
TAGS := ${addprefix ${TAG}:,${shell git describe --tags HEAD} latest}
endif

.PHONY: docker-build
docker-build: Dockerfile
	docker build ${addprefix -t,${TAGS}} .

.PHONY: docker-run
docker-run: docker-build
	docker run $(TAG) #-p 0:80 ${TAG}

.PHONY: docker-push
docker-push: docker-build
	for t in ${TAGS}; do docker push $$t; done

.PHONY: services-push
services-push:
	${MAKE} -C services docker-push

.PHONY: docker-compose-build
docker-compose-build:
	cd misc/docker && PCW_BASE_DIR=${PCW_SRV_DIR} docker-compose build

.PHONY: docker-compose-pull
docker-compose-pull:
	cd misc/docker && PCW_BASE_DIR=${PCW_SRV_DIR} docker-compose pull

.PHONY: docker-deploy
docker-deploy: install-frontend docker-push services-push docker-compose-pull docker-compose-build
	cd misc/docker && PCW_BASE_DIR=${PCW_SRV_DIR} docker-compose up -d

.PHONY: install-frontend
install-frontend:
	$(MAKE) -C frontend INSTALL_DIR=${PCW_SRV_DIR}/www-data install

.PHONY: docker-stop
docker-stop:
	cd misc/docker && PCW_BASE_DIR=${PCW_SRV_DIR} docker-compose stop
