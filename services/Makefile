SUBMAKES = ${wildcard */Makefile}

docker-push docker-build clean: $(SUBMAKES)

.PHONY: $(SUBMAKES)
${SUBMAKES}:
	$(MAKE) -C ${shell dirname $@} $(MAKECMDGOALS)
