PCW_API_VERSION_MAJOR := 0
PCW_API_VERSION_MINOR := 1
PCW_API_VERSION_PATCH := 0

CXX ?= g++

CXXFLAGS := -MP -MD -std=gnu++14 -Wall -Werror -fpic
CXXFLAGS += -Irest/src
CXXFLAGS += -isystem modules/sqlpp11/include
CXXFLAGS += -isystem modules/sqlpp11-connector-mysql/include
CXXFLAGS += -isystem modules/date
CXXFLAGS += -isystem modules/crow/include
CXXFLAGS += -isystem modules/utfcpp/source
CXXFLAGS += -isystem modules/pugixml/src
CXXFLAGS += -DPCW_API_VERSION_MAJOR=$(PCW_API_VERSION_MAJOR)
CXXFLAGS += -DPCW_API_VERSION_MINOR=$(PCW_API_VERSION_MINOR)
CXXFLAGS += -DPCW_API_VERSION_PATCH=$(PCW_API_VERSION_PATCH)

LDFLAGS += -Llib
LDFLAGS += -lpcwcore -lpcwapi -lpcwparser -lpugixml
LDFLAGS += -lpcwcore -lpcwapi -lpcwparser -lpugixml # do it two times
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

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin
LIBDIR := $(PREFIX)/lib
DISTDIR ?=

PCW_DB_HOST ?= localhost
PCW_DB_USER ?= pocoweb
PCW_DB_PASS ?= pocoweb1998
PCW_DB ?= pocoweb
PCW_DB_CONNECTIONS ?= 10
PCW_DB_DEBUG ?= true

PCW_API_USER ?= pocoweb
PCW_API_PASS ?= pocoweb123
PCW_API_EMAIL ?= pocoweb@cis.lmu.de
PCW_API_INSTITUTE ?= CIS

-include make/cache.mak

make/cache.mak: Makefile make/config.mak
	@echo "Generating $@"
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
	@echo "DISTDIR := $(DISTDIR)" >> $@
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

config.ini: misc/default/config.def.ini make/cache.mak
	@echo "Generating $@"
	@sed -e 's/$$[({]PCW_DB_HOST[})]/$(PCW_DB_HOST)/g' \
	     -e 's/$$[({]PCW_DB_USER[})]/$(PCW_DB_USER)/g' \
	     -e 's/$$[({]PCW_DB_PASS[})]/$(PCW_DB_PASS)/g' \
	     -e 's/$$[({]PCW_DB[})]/$(PCW_DB)/g' \
	     -e 's/$$[({]PCW_DB_DEBUG[})]/$(PCW_DB_DEBUG)/g' \
	     -e 's/$$[({]PCW_DB_CONNECTIONS[})]/$(PCW_DB_CONNECTIONS)/g' \
	     -e 's/$$[({]PCW_API_USER[})]/$(PCW_API_USER)/g' \
	     -e 's/$$[({]PCW_API_PASS[})]/$(PCW_API_PASS)/g' \
	     -e 's/$$[({]PCW_API_EMAIL[})]/$(PCW_API_EMAIL)/g' \
	     -e 's/$$[({]PCW_API_INSTITUTE[})]/$(PCW_API_INSTITUTE)/g' \
	     $< > $@

ALL += config.ini
