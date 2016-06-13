include ../config.mk

default: main config.ini

CXXFLAGS ?= -Wall -Werror -std=gnu++14 -g -Og
CXXFLAGS := $(CXXFLAGS) -MP -MD

CXXFLAGS += -I src/util

LDFLAGS ?=
LDFLAGS := $(LDFLAGS)
LDFLAGS += -l mysqlcppconn
#LDFLAGS += $(shell pkgconf --libs openssl)
LDFLAGS += -l crypt

VPATH = src/db src/util

OBJS += User.o
OBJS += Config.o
OBJS += DbTableUsers.o
OBJS += main.o

DEPS = $(patsubst %.o,%.d,$(OBJS))
-include $(DEPS)

main: librest.a
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

librest.a: $(OBJS)
	$(AR) rcs $@ $^

config.ini:
	cp ../misc/default/config.def.ini $@
	sed -i -e "s/@PCW_DB_USER@/$(PCW_DB_USER)/g" $@
	sed -i -e "s/@PCW_DB_HOST@/$(PCW_DB_HOST)/g" $@
	sed -i -e "s/@PCW_DB_PASS@/$(PCW_DB_PASS)/g" $@

.PHONY: clean
clean:
	$(RM) *.o *.d librest.a main config.ini
