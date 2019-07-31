SUDO ?= sudo
TAG ?= flobar/pocoweb
TAGS := ${addprefix ${TAG}:,${shell git describe --tags HEAD} latest}

.PHONY: docker-build
docker-build: Dockerfile
	${SUDO} docker build ${addprefix -t,${TAGS}} .

.PHONY: docker-run
docker-run: docker-build
	${SUDO} docker run -p 0:80 ${TAG}

.PHONY: docker-push
docker-push: docker-build
	for t in ${TAGS}; do ${SUDO} docker push $$t; done
