export CXXFLAGS
export LDFLAGS += -L ../../rest -lpcwcore -lpcwparser
export MAKEFLAGS

test:
all clean:
	$(MAKE) -C example $@
	$(MAKE) -C simple-login $@
	$(MAKE) -C profiler $@
