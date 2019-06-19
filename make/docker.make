SUDO ?= sudo
TAG ?= flobar/pocoweb

.PHONY: docker-build
docker-build: Dockerfile
	${SUDO} docker build -t ${TAG} .

.PHONY: docker-run
docker-run: docker-build
	${SUDO} docker run -p 0:80 ${TAG}

.PHONY: docker-push
docker-push: docker-build
	${SUDO} docker push ${TAG}
