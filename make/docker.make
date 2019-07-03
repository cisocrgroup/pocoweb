SUDO ?= sudo
TAG ?= flobar/pocoweb
TAGS := ${addprefix -t${TAG}:,${shell git describe --tags HEAD} latest}

.PHONY: docker-build
docker-build: Dockerfile
	${SUDO} docker build ${TAGS} .

.PHONY: docker-run
docker-run: docker-build
	${SUDO} docker run -p 0:80 ${TAG}

.PHONY: docker-push
docker-push: docker-build
	${SUDO} docker push ${TAG}
