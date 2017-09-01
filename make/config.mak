PCW_API_VERSION_MAJOR := 0
PCW_API_VERSION_MINOR := 5
PCW_API_VERSION_PATCH := 0

V ?= @
C ?= yes
DESTDIR ?=
PREFIX ?= /usr/local
BINDIR := $(DESTDIR)/$(PREFIX)/bin
LIBDIR := $(DESTDIR)/$(PREFIX)/lib

CXX ?= g++

CXXFLAGS := -MP -MD -std=gnu++14 -Wall -Werror -fpic
CXXFLAGS += -g
CXXFLAGS += -Irest/src
CXXFLAGS += -Wundef
CXXFLAGS += -Wnoexcept
CXXFLAGS += -Wno-aggressive-loop-optimizations
CXXFLAGS += -Wsuggest-override
CXXFLAGS += -Wsuggest-attribute=pure
CXXFLAGS += -Wsuggest-attribute=const
CXXFLAGS += -Wsuggest-attribute=noreturn
CXXFLAGS += -isystem modules/sqlpp11/include
CXXFLAGS += -isystem modules/sqlpp11-connector-mysql/include
CXXFLAGS += -isystem modules/date
CXXFLAGS += -isystem modules/crow/include
CXXFLAGS += -isystem modules/utfcpp/source
CXXFLAGS += -isystem modules/pugixml/src
CXXFLAGS += -DPCW_API_VERSION_MAJOR=$(PCW_API_VERSION_MAJOR)
CXXFLAGS += -DPCW_API_VERSION_MINOR=$(PCW_API_VERSION_MINOR)
CXXFLAGS += -DPCW_API_VERSION_PATCH=$(PCW_API_VERSION_PATCH)
CXXFLAGS += -DPCW_DESTDIR=$(DESTDIR)

LDFLAGS += -Llib
LDFLAGS += -lcurl
LDFLAGS += -lpcw
LDFLAGS += -lpugixml
LDFLAGS += -ldl
LDFLAGS += -lssl
LDFLAGS += -llept
LDFLAGS += -lpthread
LDFLAGS += -licuuc
LDFLAGS += -lboost_system
LDFLAGS += -lboost_filesystem
LDFLAGS += -lcrypto
LDFLAGS += -lcrypt
LDFLAGS += -lsqlpp-mysql
LDFLAGS += -lmysqlclient

PCW_DB_HOST ?= localhost
PCW_DB_USER ?= pocoweb
PCW_DB_PASS ?= pocoweb1998
PCW_DB ?= pocoweb
PCW_DB_CONNECTIONS ?= 10
PCW_DB_DEBUG ?= false

PCW_API_USER ?= pocoweb
PCW_API_PASS ?= pocoweb123
PCW_API_EMAIL ?= pocoweb@cis.lmu.de
PCW_API_INSTITUTE ?= CIS

PCW_LOG_PIDFILE ?= /var/run/pocoweb.pid

ECHO = 							\
	@if [ "$C" = "yes" ]; then 			\
		echo -e "[\033[0;32m$1\033[0m]";	\
	else						\
		echo "[$1]";				\
	fi

%.o: %.cpp
	$(call ECHO,$@)
	$V $(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<
mkdir-%:; @mkdir -p $(subst -,/,$*)
dbg-%:; @echo $*: ${$*}

-include make/cache.mak

make/cache.mak: Makefile
	$(call ECHO,$@)
	@echo "#" > $@
	@echo "# cache.mak" >> $@
	@echo "# created: `date`" >> $@
	@echo "#" >> $@
	@echo "PCW_API_VERSION_MAJOR := $(PCW_API_VERSION_MAJOR)" >> $@
	@echo "PCW_API_VERSION_MINOR := $(PCW_API_VERSION_MINOR)" >> $@
	@echo "PCW_API_VERSION_PATCH := $(PCW_API_VERSION_PATCH)" >> $@
	@echo "CXX := $(CXX)" >> $@
	@echo "CXXFLAGS := $(CXXFLAGS)" >> $@
	@echo "LDFLAGS := $(LDFLAGS)" >> $@
	@echo "FPIC := $(FPIC)" >> $@
	@echo "PREFIX := $(PREFIX)" >> $@
	@echo "BINDIR := $(BINDIR)" >> $@
	@echo "LIBDIR := $(LIBDIR)" >> $@
	@echo "DESTDIR := $(DESTDIR)" >> $@
	@echo "PCW_DB_HOST := $(PCW_DB_HOST)" >> $@
	@echo "PCW_DB_USER := $(PCW_DB_USER)" >> $@
	@echo "PCW_DB_PASS := $(PCW_DB_PASS)" >> $@
	@echo "PCW_DB := $(PCW_DB)" >> $@
	@echo "PCW_DB_CONNECTIONS := $(PCW_DB_CONNECTIONS)" >> $@
	@echo "PCW_DB_DEBUG := $(PCW_DB_DEBUG)" >> $@
	@echo "PCW_API_USER:= $(PCW_API_USER)" >> $@
	@echo "PCW_API_PASS:= $(PCW_API_PASS)" >> $@
	@echo "PCW_API_EMAIL:= $(PCW_API_EMAIL)" >> $@
	@echo "PCW_API_INSTITUTE := $(PCW_API_INSTITUTE)" >> $@
	@echo "PCW_LOG_PIDFILE := $(PCW_LOG_PIDFILE)" >> $@

