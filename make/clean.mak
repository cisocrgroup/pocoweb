
clean:
	find . -type f -a \( \
		-name '*.o' -o \
		-name '*.d' -o \
		-name '*.a' -o \
		-name '*.so' -o \
		-name '*.test' -o \
		-name '*.gitmodule' \
		\) -delete

clean-cache:
	$(RM) make/cache.mak config.ini

clean-all: clean clean-cache

.PHONY: clean clean-cache clean-all
