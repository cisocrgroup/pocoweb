PCW_SRV_DIR ?= /srv/pocoweb
SUDO ?= sudo
TAG ?= flobar/pocoweb
ifeq (, ${shell which git})
TAGS := latest
else
TAGS := ${addprefix ${TAG}:,${shell git describe --tags HEAD} latest}
endif

.PHONY: docker-build
docker-build: Dockerfile
	${SUDO} docker build ${addprefix -t,${TAGS}} .

.PHONY: docker-run
docker-run: docker-build
	${SUDO} docker run $(TAG) #-p 0:80 ${TAG}

.PHONY: docker-push
docker-push: docker-build
	for t in ${TAGS}; do ${SUDO} docker push $$t; done

.PHONY: services-push
services-push:
	${MAKE} -C services docker-push

.PHONY: docker-deploy
docker-deploy: docker-push services-push
	cd misc/docker && ${SUDO} PCW_BASE_DIR=${PCW_SRV_DIR} docker-compose pull
	cd misc/docker && ${SUDO} PCW_BASE_DIR=${PCW_SRV_DIR} docker-compose up
