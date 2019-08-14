TARGET := pcwocr
SUDO ?= sudo
TAG ?= flobar/${TARGET}
PORTS ?= 8080:80
TAGS := ${addprefix ${TAG}:,${shell git describe --tags HEAD} latest}
SRCS := ${shell find . -type f -name '*.go'}

default: ${TARGET}

${TARGET}: ${SRCS}
	go build .

.PHONY: docker-build
docker-build: ${TARGET}
	${SUDO} docker build ${addprefix -t,${TAGS}} .

.PHONY: docker-run
docker-run: docker-build
	${SUDO} docker run -p ${PORTS} ${TAG}

.PHONY: docker-push
docker-push: docker-build
	for t in ${TAGS}; do ${SUDO} docker push $$t; done

.PHONY: clean
clean:
	$(RM) ${TARGET}
