
clean:
	find . -type f -a \( \
		-name '*.o' -o \
		-name '*.d' -o \
		-name '*.mod' -o \
		-name '*.a' -o \
		-name '*.so' -o \
		-name '*.test' \
		\) -delete

clean-cache:
	$(RM) make/cache.mak config.ini

clean-all: clean clean-cache

.PHONY: clean clean-cache clean-all
