SUDO ?= sudo
TAG ?= flobar/pocoweb
PORTS ?= 80:80

.PHONY: docker-build
docker-build: Dockerfile
	${SUDO} docker build -t ${TAG} .

.PHONY: docker-push
docker-push: docker-build
	${SUDO} docker push ${TAG}
