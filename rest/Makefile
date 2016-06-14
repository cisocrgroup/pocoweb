include ../config.mk

CXXFLAGS ?= -Wall -Werror -std=gnu++14 -g -Og
CXXFLAGS := $(CXXFLAGS) -MP -MD
CXXFLAGS += -I src
CXXFLAGS += -I gsm/Simple-Web-Server

LDFLAGS ?=
LDFLAGS := $(LDFLAGS)
LDFLAGS += -l mysqlcppconn
LDFLAGS += -l boost_system
LDFLAGS += -l boost_coroutine
LDFLAGS += -l boost_regex
LDFLAGS += -l pthread
LDFLAGS += -l crypt

VPATH = src/db src/util src/api src

OBJS += User.o
OBJS += Config.o
OBJS += DbTableUsers.o
OBJS += Login.o
OBJS += pocowebd.o

DEPS = $(patsubst %.o,%.d,$(OBJS))
-include $(DEPS)

default: pocowebd config.ini

pocowebd: libpocoweb.a
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

libpocoweb.a: $(OBJS)
	$(AR) rcs $@ $^

config.ini:
	cp ../misc/default/config.def.ini $@
	sed -i -e "s/@PCW_DB_USER@/$(PCW_DB_USER)/g" $@
	sed -i -e "s/@PCW_DB_HOST@/$(PCW_DB_HOST)/g" $@
	sed -i -e "s/@PCW_DB_PASS@/$(PCW_DB_PASS)/g" $@
	sed -i -e "s/@PCW_DAEMON_HOST@/$(PCW_DAEMON_HOST)/g" $@
	sed -i -e "s/@PCW_DAEMON_PORT@/$(PCW_DAEMON_PORT)/g" $@
	sed -i -e "s/@PCW_DAEMON_THREADS@/$(PCW_DAEMON_THREADS)/g" $@

.PHONY: clean
clean:
	$(RM) *.o *.d libpocoweb.a main config.ini
