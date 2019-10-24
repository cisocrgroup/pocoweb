GITTAG := $(shell git describe --tags HEAD)
DOCKERFILES := $(wildcard */Dockerfile)

.PHONY: docker-build
docker-build: $(addsuffix .build,$(filter-out pcwocr%,$(DOCKERFILES)))
.PHONY: docker-push
docker-push: $(addsuffix .build,$(filter-out nginx%,$(filter-out pcwocr%,$(DOCKERFILES))))

%.build:
	docker build -t $(call mktag,$@,$(GITTAG)) -t $(call mktag,$@,latest) $(dir $@)
%.push: %.build
	docker push $(call mktag,$@,$(GITTAG))
	docker push $(call mktag,$@,latest)
mktag = flobar/$(patsubst %/,%,$(dir $1)):$2
