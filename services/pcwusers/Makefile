TARGET := pcwusers
TAG ?= flobar/${TARGET}
PORTS ?= 8080:80
TAGS := ${addprefix ${TAG}:,${shell git describe --tags HEAD} latest}
SRCS := ${shell find . -type f -name '*.go'}

default: ${TARGET}

${TARGET}: ${SRCS}
	go build .

.PHONY: docker-build
docker-build:
	docker build ${addprefix -t,${TAGS}} .

.PHONY: docker-run
docker-run: docker-build
	docker run -p ${PORTS} ${TAG}

.PHONY: docker-push
docker-push: docker-build
	for t in ${TAGS}; do docker push $$t; done

.PHONY: clean
clean:
	$(RM) ${TARGET}
