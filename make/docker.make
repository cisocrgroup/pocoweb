SUDO ?= sudo
TAG ?= flobar/pocoweb
PORTS ?= 80:80

.PHONY: docker-build
docker-build: Dockerfile
	${SUDO} docker build -t ${TAG} .

.PHONY: docker-run
docker-run: docker-build
	${SUDO} docker run -p 0:80 ${PORTS}

.PHONY: docker-push
docker-push: docker-build
	${SUDO} docker push ${TAG}
